// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Socket.h"
#include "SocketSet.h"
#include "UPnP.h"
#include "s25util/Log.h"
#include "s25util/StringConversion.h"
#include "s25util/strFuncs.h"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>

#ifdef _WIN32
#    if defined(__CYGWIN__) || defined(__MINGW32__)
#        ifndef AI_ALL
#            define AI_ALL 0x0010
#        endif
#        ifndef AI_ADDRCONFIG
#            define AI_ADDRCONFIG 0x0020
#        endif
#        ifndef AI_V4MAPPED
#            define AI_V4MAPPED 0x0800
#        endif
#    endif
#else
#    include <arpa/inet.h>
#    include <cerrno>
#    include <netdb.h>
#    include <netinet/tcp.h>
#    include <sys/ioctl.h>
#    include <unistd.h>
#endif

// do not use addr resolution for localhost
ResolvedAddr::ResolvedAddr(const HostAddr& hostAddr, bool resolveAll) : lookup((hostAddr.host != "localhost"))
{
    if(lookup)
    {
        addrinfo hints;
        memset(&hints, 0, sizeof(addrinfo));
        if(resolveAll)
        {
            hints.ai_flags = AI_ADDRCONFIG;
#ifndef __FreeBSD__
            // Defined, but getaddrinfo complains about it on FreeBSD -> Check again with the combination with
            // AI_V4MAPPED
            hints.ai_flags |= AI_ALL | AI_V4MAPPED;
#endif
        } else
            hints.ai_flags = AI_NUMERICHOST;

        hints.ai_socktype = hostAddr.isUDP ? SOCK_DGRAM : SOCK_STREAM;

        if(hostAddr.ipv6)
            hints.ai_family = AF_INET6;
        else
            hints.ai_family = AF_INET;

        int error = getaddrinfo(hostAddr.host.c_str(), hostAddr.port.c_str(), &hints, &addr);
        if(error != 0)
        {
#ifdef _WIN32
            std::cerr << "getaddrinfo: " << gai_strerrorA(error) << "\n";
#else
            std::cerr << "getaddrinfo: " << gai_strerror(error) << "\n";
#endif // _WIN32
            addr = nullptr;
        }
    } else // fill with loopback
    {
        addr = new addrinfo;
        addr->ai_family = (hostAddr.ipv6 ? AF_INET6 : AF_INET);
        addr->ai_addrlen = (hostAddr.ipv6 ? sizeof(sockaddr_in6) : sizeof(sockaddr_in));
        addr->ai_socktype = hostAddr.isUDP ? SOCK_DGRAM : SOCK_STREAM;
        addr->ai_addr = (sockaddr*)calloc(1, addr->ai_addrlen);
        if(!addr->ai_addr)
            throw std::bad_alloc();

        if(hostAddr.ipv6)
        {
            auto* addr6 = reinterpret_cast<sockaddr_in6*>(addr->ai_addr);
            addr6->sin6_family = AF_INET6;
            addr6->sin6_port = htons(s25util::fromStringClassic<unsigned short>(hostAddr.port));
            addr6->sin6_addr = in6addr_loopback;
        } else
        {
            auto* addr4 = reinterpret_cast<sockaddr_in*>(addr->ai_addr);
            addr4->sin_family = AF_INET;
            addr4->sin_port = htons(s25util::fromStringClassic<unsigned short>(hostAddr.port));
            addr4->sin_addr.s_addr = inet_addr("127.0.0.1");
        }
    }
}

ResolvedAddr::~ResolvedAddr()
{
    if(!addr)
        return;
    if(lookup)
        freeaddrinfo(addr);
    else
    {
        free(addr->ai_addr);
        delete addr;
    }
}

PeerAddr::PeerAddr(unsigned short port)
{
    addr.sa_in.sin_family = AF_INET;
    addr.sa_in.sin_addr.s_addr = INADDR_BROADCAST;
    addr.sa_in.sin_port = htons(port);
}

std::string PeerAddr::GetIp() const
{
    return Socket::IpToString(GetAddr());
}

sockaddr* PeerAddr::GetAddr()
{
    return &addr.sa;
}

const sockaddr* PeerAddr::GetAddr() const
{
    return &addr.sa;
}

Socket::Socket()
    : socket_(INVALID_SOCKET), refCount_(nullptr), status_(Status::Invalid), isBroadcast(false), upnpPort_(0)
{}

/**
 *  Konstruktor von @p Socket.
 *
 *  @param[in] so Socket welches benutzt werden soll
 *  @param[in] st Status der gesetzt werden soll
 */
Socket::Socket(const SOCKET so, Status st)
    : socket_(so), refCount_(new int32_t), status_(st), isBroadcast(false), upnpPort_(0)
{
    *refCount_ = 1;
}

Socket::Socket(const Socket& so)
    : socket_(so.socket_), refCount_(so.refCount_), status_(so.status_), isBroadcast(so.isBroadcast), upnpPort_(0)
{
    if(refCount_)
        ++*refCount_;
}

Socket::Socket(Socket&& so) noexcept
    : socket_(std::exchange(so.socket_, INVALID_SOCKET)), refCount_(std::exchange(so.refCount_, nullptr)),
      status_(std::exchange(so.status_, Status::Invalid)), isBroadcast(so.isBroadcast),
      upnpPort_(std::exchange(so.upnpPort_, 0))
{}

Socket::~Socket()
{
    Close();
}

Socket& Socket::operator=(Socket rhs)
{
    swap(*this, rhs);
    return *this;
}

/**
 *  Setzt ein Socket auf übergebene Werte.
 *
 *  @param[in] so Socket welches benutzt werden soll
 *  @param[in] st Status der gesetzt werden soll
 */
void Socket::Set(const SOCKET socket, Status status)
{
    socket_ = socket;
    status_ = status;
}

/**
 *  Initialisiert die Socket-Bibliothek.
 *
 *  @return @p true bei Erfolg, @p false bei Fehler
 */
bool Socket::Initialize()
{
#ifdef _WIN32
    // obligatorische WSAStartup unter Windows
    WSAData wsa;
    if(WSAStartup(MAKEWORD(2, 0), &wsa))
        return false;
#endif // _WIN32

    // Erfolg melden
    return true;
}

/**
 *  räumt die Socket-Bibliothek auf.
 */
void Socket::Shutdown()
{
#ifdef _WIN32
    // obligatorische WSACleanup unter Windows
    WSACleanup();
#endif // _WIN32
}

/**
 *  erstellt und initialisiert das Socket.
 *
 *  @return @p true bei Erfolg, @p false bei Fehler
 */
bool Socket::Create(int family, bool asUDPBroadcast)
{
    // socket ggf. schließen
    Close();

    socket_ = asUDPBroadcast ? socket(family, SOCK_DGRAM, IPPROTO_UDP) : socket(family, SOCK_STREAM, 0);

    // Ist es gültig?
    status_ = (INVALID_SOCKET == socket_ ? Status::Invalid : Status::Valid);

    if(asUDPBroadcast)
    {
        int enable = 1;
        SetSockOpt(SO_BROADCAST, &enable, sizeof(enable), SOL_SOCKET);
    } else
    {
        // Nagle deaktivieren
        int disable = 1;
        SetSockOpt(TCP_NODELAY, &disable, sizeof(int), IPPROTO_TCP);

        int enable = 1;
        SetSockOpt(SO_REUSEADDR, &enable, sizeof(int), SOL_SOCKET);
    }

    refCount_ = new int32_t(1); //-V508
    isBroadcast = asUDPBroadcast;

    return (status_ != Status::Invalid);
}

/**
 *  schliesst das Socket.
 */
void Socket::Close()
{
    // Check if we have a socket
    if(!refCount_)
    {
        assert(status_ == Status::Invalid && socket_ == INVALID_SOCKET);
        return;
    }
    // Decrease ref count (not thread safe!)
    --*refCount_;
    if(*refCount_ <= 0)
    {
        // Physically close socket, if no references left
        if(socket_ != INVALID_SOCKET)
            closesocket(socket_);
        delete refCount_;

        if(upnpPort_)
        {
            try
            {
                UPnP::ClosePort(upnpPort_);
            } catch(const std::runtime_error& e)
            {
                LOG.write("Failed to renove UPnP portforwarding: %1%") % e.what();
            }
        }
    }

    // Cleanup (even if the socket is still open, we just don't "have" it anymore)
    Set(INVALID_SOCKET, Status::Invalid);
    refCount_ = nullptr;
}

bool Socket::Bind(unsigned short port, bool useIPv6) const
{
    union
    {
        sockaddr_storage addrs;
        sockaddr_in addrV4;
        sockaddr_in6 addrV6;
    };
    memset(&addrs, 0, sizeof(sockaddr_storage));

    int size;
    if(useIPv6)
    {
        addrV6.sin6_family = AF_INET6;
        addrV6.sin6_port = htons(port);
        addrV6.sin6_addr = in6addr_any;
        size = sizeof(addrV6);
    } else
    {
        addrV4.sin_family = AF_INET;
        addrV4.sin_port = htons(port);
        addrV4.sin_addr.s_addr = INADDR_ANY;
        size = sizeof(addrV4);
    }

    return bind(socket_, (sockaddr*)&addrs, size) != SOCKET_ERROR;
}

/**
 *  setzt das Socket auf Listen.
 *
 *  @param[in] port Port auf dem "gelauscht" werden soll.
 *
 *  @return @p true bei Erfolg, @p false bei Fehler
 */
bool Socket::Listen(unsigned short port, bool use_ipv6, bool use_upnp)
{
    bool ipv6 = use_ipv6;
    bool error = false;
    int versuch = 0;

    do
    {
        // Bei Fehler jeweils nochmal mit ipv4 probieren
        if(!Create(ipv6 ? AF_INET6 : AF_INET))
        {
            ipv6 = !ipv6;
            error = true;
            ++versuch;
            if(versuch > 1)
                return false;

            continue;
        }

        // Binden
        if(!Bind(port, ipv6))
        {
            ipv6 = !ipv6;
            error = true;
            ++versuch;
            if(versuch > 1)
                return false;

            continue;
        }

        // und Horchen
        if(listen(socket_, 10))
        {
            ipv6 = !ipv6;
            error = true;
            ++versuch;
            if(versuch > 1)
                return false;

            continue;
        }

        error = false;
    } while(error);

    // try to open portforwarding if we're using ipv4
    if(use_upnp && !ipv6)
    {
        try
        {
            UPnP::OpenPort(port);
        } catch(const std::runtime_error& e)
        {
            LOG.write("Failed to forward port via UPnP: %1%") % e.what();
        }
    }

    // Status setzen
    status_ = Status::Listen;

    // und Alles gut :-)
    return true;
}

/**
 *  akzeptiert eingehende Verbindungsversuche.
 *
 *  @param[in,out] client Referenz auf Clientsocket der neuen Verbindung
 *
 *  @return @p true bei Erfolg, @p false bei Fehler
 */
Socket Socket::Accept()
{
    if(status_ != Status::Listen)
        return Socket();

    // Verbindung annehmen
    SOCKET tmp = accept(socket_, nullptr, nullptr);
    if(tmp == INVALID_SOCKET)
        return Socket();

    // Nagle deaktivieren
    int disable = 1;
    setsockopt(tmp, IPPROTO_TCP, TCP_NODELAY, (char*)&disable, sizeof(int));

    // Status setzen
    Socket client = Socket(tmp, Status::Connected);

    // Alles gut :-)
    return client;
}

/**
 *  liefert Ip-Adresse(n) für einen Hostnamen.
 *
 *  @param[in] hostname Ziel-Hostname/-ip
 */
std::vector<HostAddr> Socket::HostToIp(const std::string& hostname, const unsigned port, bool get_ipv6, bool useUDP)
{
    std::vector<HostAddr> ips;
    std::string sPort = s25util::toStringClassic(port);

    HostAddr hostAddr;
    hostAddr.host = hostname;
    hostAddr.port = sPort;
    hostAddr.ipv6 = get_ipv6;
    hostAddr.isUDP = useUDP;

    // no dns resolution for localhost
    if(hostname == "localhost")
    {
        ips.push_back(hostAddr);
        return ips;
    }

    ResolvedAddr res(hostAddr, true);
    if(!res.isValid())
        return ips;

    addrinfo* addr = &res.getAddr();
    do
    {
        HostAddr h;
        h.host = IpToString(addr->ai_addr);
        h.port = sPort;
        h.ipv6 = (addr->ai_family == AF_INET6);
        h.isUDP = addr->ai_protocol == SOCK_DGRAM;
        ips.push_back(h);

        addr = addr->ai_next;
    } while(addr != nullptr);

    return ips;
}

void Socket::Sleep(unsigned ms)
{
#ifdef _WIN32
    ::Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

/**
 *  versucht eine Verbindung mit einem externen Host aufzubauen.
 *
 *  @param[in] hostname Ziel-Hostname/-ip
 *  @param[in] port     Port zu dem Verbunden werden soll
 *
 *  @p true bei Erfolg, @p false bei Fehler
 */
bool Socket::Connect(const std::string& hostname, const unsigned short port, bool use_ipv6, const ProxySettings& proxy)
{
    if(proxy.type == ProxyType::Socks4)
        use_ipv6 = false;

    std::vector<HostAddr> proxy_ips;
    if(proxy.type != ProxyType::None)
    {
        proxy_ips = HostToIp(proxy.hostname, proxy.port, use_ipv6);
        if(proxy_ips.empty())
            return false;
    }

    // TODO: socks v5 kann remote resolven
    std::vector<HostAddr> ips = HostToIp(hostname, port, use_ipv6);
    if(ips.empty())
        return false;

    bool done = false;

    // do not use proxy for connecting to localhost
    const std::vector<HostAddr>& usedIPs = (proxy.type != ProxyType::None && hostname != "localhost") ? proxy_ips : ips;

    for(const auto& it : usedIPs)
    {
        if(it.isUDP)
            throw std::invalid_argument("Cannot connect to UDP (yet)");

        if(!Create(it.ipv6 ? AF_INET6 : AF_INET))
            continue;

        // aktiviere non-blocking
        unsigned long argp = 1;
#ifdef _WIN32
        ioctlsocket(socket_, FIONBIO, &argp);
#else
        ioctl(socket_, FIONBIO, &argp);
#endif

        ResolvedAddr addr(it);
        if(!addr.isValid())
        {
            LOG.write("Could not resolve %s. Skipping...\n") % it.host;
            continue;
        }
        std::string ip = IpToString(addr.getAddr().ai_addr); //-V807
        LOG.write("Connection to %s%s:%d\n") % (proxy.type != ProxyType::None ? "Proxy " : "") % ip
          % (proxy.type != ProxyType::None ? proxy.port : port);

        // Und schließlich Verbinden
        if(connect(socket_, addr.getAddr().ai_addr, static_cast<int>(addr.getAddr().ai_addrlen)) != SOCKET_ERROR)
        {
            done = true;
            break;
        }
#ifdef _WIN32
        if(WSAGetLastError() == WSAEWOULDBLOCK)
#else
        if(errno == EINPROGRESS || errno == EWOULDBLOCK)
#endif
        {
            int timeout = 0;
            while(!done)
            {
                SocketSet sw, se;
                sw.Add(*this);
                se.Add(*this);

                if(sw.Select(0, 1) == 1 || se.Select(0, 2) != 0)
                {
                    unsigned err;
                    socklen_t len = sizeof(unsigned);
                    getsockopt(socket_, SOL_SOCKET, SO_ERROR, (char*)&err, &len); //-V206

                    if(err != 0)
                    {
#ifdef _WIN32
                        WSASetLastError(err);
#else
                        errno = err;
#endif
                        break;
                    }

                    switch(proxy.type)
                    {
                        default: break;
                        case ProxyType::Socks4:
                        {
                            std::array<char, 18> proxyinit;

                            proxyinit[0] = 4; // socks v4
                            proxyinit[1] = 1; // 1=connect
                            const uint16_t nwOrderPort = htons(port);
                            std::memcpy(&proxyinit[2], &nwOrderPort, sizeof(nwOrderPort));
                            for(const auto& hostAddr : ips)
                            {
                                if(!hostAddr.ipv6
                                   && sscanf(hostAddr.host.c_str(), "%c.%c.%c.%c", &proxyinit[4], &proxyinit[5],
                                             &proxyinit[6], &proxyinit[7])
                                        == 4)
                                    break;
                            }
                            strcpy_check(proxyinit, 8, "siedler25"); // userid

                            Send(proxyinit);

                            int proxy_timeout = 0;
                            while(BytesWaiting() < 8 && proxy_timeout < 8)
                            {
                                Sleep(250);
                                ++proxy_timeout;
                            }

                            if(proxy_timeout >= 8)
                            {
                                LOG.write("Proxy error: connection timed out\n");
                                return false;
                            }

                            Recv(proxyinit, 8);

                            if(proxyinit[0] != 0 || proxyinit[1] != 90)
                            {
                                LOG.write("Proxy error: got %d: connection rejected or failed or other error\n")
                                  % proxyinit[1];
                                return false;
                            }
                        }
                        break;
                        case ProxyType::Socks5:
                        {
                            // not implemented
                            return false;
                        }
                        break;
                    }

                    // Verbindung hergestellt
                    done = true;
                }

                Sleep(50);

                ++timeout;
                if(timeout > 8)
                {
#ifdef _WIN32
                    WSASetLastError(WSAETIMEDOUT);
#else
                    errno = ETIMEDOUT;
#endif
                    break;
                }
            }
            if(done)
                break;
        }
        LOG.writeLastError("Connection failed\nError");
    }

    if(!done)
    {
        LOG.write("Error connection to %s:%d\n") % hostname % port;
        return false;
    }
    LOG.write("Successfully connected to %s:%d\n") % hostname % port;

    // deaktiviere non-blocking
    unsigned long argp = 0;
#ifdef _WIN32
    ioctlsocket(socket_, FIONBIO, &argp);
#else
    ioctl(socket_, FIONBIO, &argp);
#endif

    status_ = Status::Connected;

    // Alles ok
    return true;
}

/**
 *  liest Daten vom Socket in einen Puffer.
 *
 *  @param[in] buffer Zielpuffer in den gelesen werden soll
 *  @param[in] length Anzahl an Bytes die gelesen werden soll
 *  @param[in] block  Soll der Vorgang blockierend sein?
 *
 *  @return -1 bei Fehler, Anzahl der empfangenen Bytes (max. @p length )
 */
int Socket::Recv(void* buffer, const int length, bool block) const
{
    if(!isValid())
        return -1;

    // und empfangen
    return recv(socket_, reinterpret_cast<char*>(buffer), length, (block ? 0 : MSG_PEEK));
}

int Socket::Recv(void* const buffer, const int length, PeerAddr& addr) const
{
    if(!isValid())
        return -1;

    socklen_t addrLen = addr.GetSize();
    return recvfrom(socket_, reinterpret_cast<char*>(buffer), length, 0, addr.GetAddr(), &addrLen);
}

/**
 *  schreibt Daten von einem Puffer auf das Socket.
 *
 *  @param[in] buffer Quellpuffer aus dem gelesen werden soll
 *  @param[in] length Anzahl an Bytes die geschrieben werden soll
 *
 *  @return -1 bei Fehler, Anzahl der gesendeten Bytes (max. @p length )
 */
int Socket::Send(const void* const buffer, const int length) const
{
    if(!isValid())
        return -1;

    // und verschicken
    return send(socket_, reinterpret_cast<const char*>(buffer), length, 0);
}

int Socket::Send(const void* const buffer, const int length, const PeerAddr& addr) const
{
    if(!isValid())
        return -1;

    return sendto(socket_, reinterpret_cast<const char*>(buffer), length, 0, addr.GetAddr(), addr.GetSize());
}

/**
 *  setzt eine Socketoption.
 *
 *  @param[in] buffer Quellpuffer aus dem gelesen werden soll
 *  @param[in] length Anzahl an Bytes die geschrieben werden soll
 *
 *  @return @p true bei Erfolg, @p false bei Fehler
 */
bool Socket::SetSockOpt(int nOptionName, const void* lpOptionValue, int nOptionLen, int nLevel) const
{
    return (SOCKET_ERROR != setsockopt(socket_, nLevel, nOptionName, (const char*)lpOptionValue, nOptionLen));
}

/**
 *  Größer-Vergleichsoperator.
 *
 *  @param[in] sock Socket mit dem auf größer verglichen werden soll
 *
 *  @return liefert true falls @p this größer ist als @p sock
 */
bool Socket::operator>(const Socket& sock) const
{
    return this->socket_ > sock.socket_;
}

/**
 *  prüft auf wartende Bytes.
 *
 *  @return liefert die Menge der wartenden Bytes
 */
int Socket::BytesWaiting() const
{
    unsigned received;
    if(BytesWaiting(&received) != 0)
        return -1;

    return (int)received;
}

/**
 *  prüft auf wartende Bytes.
 *
 *  @param[in,out] dwReceived Menge der wartenden Bytes
 *
 *  @return liefert Null bei Erfolg, SOCKET_ERROR bei Fehler
 */
int Socket::BytesWaiting(unsigned* received) const
{
#ifdef _WIN32
    DWORD dwReceived;
    int retval = ioctlsocket(socket_, FIONREAD, &dwReceived);
    *received = dwReceived;
    return retval;
#else
    return ioctl(socket_, FIONREAD, received);
#endif
}

/**
 *  liefert die IP des Remote-Hosts.
 *
 *  @return liefert @p buffer zurück oder @p "" bei Fehler
 */
std::string Socket::GetPeerIP() const
{
    address_t peer;
    socklen_t length = sizeof(peer);

    // Remotehost-Adresse holen
    if(getpeername(socket_, &peer.sa, &length) == SOCKET_ERROR)
        return "";

    // in Text verwandeln
    return IpToString(&peer.sa);
}

/**
 *  liefert die IP des Lokalen-Hosts.
 *
 *  @return liefert @p buffer zurück oder @p "" bei Fehler
 */
std::string Socket::GetSockIP() const
{
    address_t peer;
    socklen_t length = sizeof(peer);

    // Localhost-Adresse holen
    if(getsockname(socket_, &peer.sa, &length) == SOCKET_ERROR)
        return "";

    // in Text verwandeln
    return IpToString(&peer.sa);
}

/**
 *  Gets a reference to the Socket
 *
 *  @return reference to @p socket_
 */
SOCKET Socket::GetSocket() const
{
    // Zeiger auf Socket liefern
    return socket_;
}

/**
 *  liefert einen string der übergebenen Ip.
 */
std::string Socket::IpToString(const sockaddr* addr)
{
    std::array<char, 256> temp{};
    address_t addrCopy;

    // a union is not initialized with zero
    std::memset(&addrCopy, 0, sizeof(addrCopy));

    size_t size;
    if(addr->sa_family == AF_INET)
    {
        size = sizeof(sockaddr_in);
        std::memcpy(&addrCopy.sa_in, addr, size);

        // set port to zero
        addrCopy.sa_in.sin_port = 0;
    } else
    {
        size = sizeof(sockaddr_in6);
        std::memcpy(&addrCopy.sa_in6, addr, size);

        // set port to zero
        addrCopy.sa_in6.sin6_port = 0;
    }

#ifdef _WIN32
    const auto le = GetLastError();
    DWORD templen = temp.size();
    WSAAddressToStringA(&addrCopy.sa, static_cast<DWORD>(size), nullptr, temp.data(), &templen);
    SetLastError(le);
#else
    const void* ip;
    if(addr->sa_family == AF_INET)
        ip = &addrCopy.sa_in.sin_addr;
    else
        ip = &addrCopy.sa_in6.sin6_addr;

    inet_ntop(addr->sa_family, ip, temp.data(), temp.size());
#endif

    return temp.data();
}
