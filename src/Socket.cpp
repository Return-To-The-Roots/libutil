// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

#include "Socket.h"
#include "Log.h"
#include "SocketSet.h"
#include "StringConversion.h"
#include "strFuncs.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>

#ifdef _WIN32
#if defined(__CYGWIN__) || defined(__MINGW32__)
#ifndef AI_ALL
#define AI_ALL 0x0010
#endif
#ifndef AI_ADDRCONFIG
#define AI_ADDRCONFIG 0x0020
#endif
#ifndef AI_V4MAPPED
#define AI_V4MAPPED 0x0800
#endif
#endif
#else
#include <arpa/inet.h>
#include <cerrno>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

ResolvedAddr::ResolvedAddr(const HostAddr& hostAddr, bool resolveAll)
{
    // do not use addr resolution for localhost
    lookup = (hostAddr.host != "localhost");

    if(lookup)
    {
        addrinfo hints;
        memset(&hints, 0, sizeof(addrinfo));
        if(resolveAll)
        {
            hints.ai_flags = AI_ADDRCONFIG;
#ifndef __FreeBSD__
            // Defined, but getaddrinfo complains about it on FreeBSD -> Check again with the combination with AI_V4MAPPED
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

        if(hostAddr.ipv6)
        {
            auto* addr6 = (sockaddr_in6*)addr->ai_addr;
            addr6->sin6_family = AF_INET6;
            addr6->sin6_port = htons(s25util::fromStringClassic<unsigned short>(hostAddr.port));
            addr6->sin6_addr = in6addr_loopback;
        } else
        {
            auto* addr4 = (sockaddr_in*)addr->ai_addr;
            addr4->sin_family = AF_INET;
            addr4->sin_port = htons(s25util::fromStringClassic<unsigned short>(hostAddr.port));
            addr4->sin_addr.s_addr = inet_addr("127.0.0.1");
        }
    }
}

ResolvedAddr::~ResolvedAddr()
{
    if(lookup)
    {
        if(addr)
        {
            freeaddrinfo(addr);
            addr = nullptr;
        }
    } else
    {
        free(addr->ai_addr);
        delete addr;
        addr = nullptr;
    }
}

PeerAddr::PeerAddr(unsigned short port)
{
    addr.ss_family = AF_INET;
    reinterpret_cast<sockaddr_in&>(addr).sin_addr.s_addr = INADDR_BROADCAST;
    reinterpret_cast<sockaddr_in&>(addr).sin_port = htons(port);
}

std::string PeerAddr::GetIp() const
{
    return Socket::IpToString(GetAddr());
}

sockaddr* PeerAddr::GetAddr()
{
    return reinterpret_cast<sockaddr*>(&addr);
}

const sockaddr* PeerAddr::GetAddr() const
{
    return reinterpret_cast<const sockaddr*>(&addr);
}

Socket::Socket() : socket_(INVALID_SOCKET), refCount_(nullptr), status_(Status::Invalid), isBroadcast(false) {}

/**
 *  Konstruktor von @p Socket.
 *
 *  @param[in] so Socket welches benutzt werden soll
 *  @param[in] st Status der gesetzt werden soll
 */
Socket::Socket(const SOCKET so, Status st) : socket_(so), refCount_(new int32_t), status_(st), isBroadcast(false)
{
    *refCount_ = 1;
}

Socket::Socket(const Socket& so) : socket_(so.socket_), refCount_(so.refCount_), status_(so.status_), isBroadcast(so.isBroadcast)
{
    if(refCount_)
        ++*refCount_;
}

Socket::~Socket()
{
    Close();
}

Socket& Socket::operator=(const Socket& rhs)
{
    if(this == &rhs)
        return *this;
    // Close our socket
    Close();
    // Get the other one
    Set(rhs.socket_, rhs.status_);
    refCount_ = rhs.refCount_;
    if(refCount_)
        ++*refCount_;
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
        upnp_.ClosePort();
        if(socket_ != INVALID_SOCKET)
            closesocket(socket_);
        delete refCount_;
    }

    // Cleanup (even if the socket is still open, we just don't "have" it anymore)
    Set(INVALID_SOCKET, Status::Invalid);
    refCount_ = nullptr;
}

bool Socket::Bind(unsigned short port, bool useIPv6)
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
    if(use_upnp)
        if(!ipv6 && !upnp_.OpenPort(port))
            LOG.writeLastError("Automatisches Erstellen des Portforwardings mit UPnP fehlgeschlagen\nFehler");

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
    SOCKET tmp = accept(socket_, 0, 0);
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
    if(proxy.type == PROXY_SOCKS4)
        use_ipv6 = false;

    std::vector<HostAddr> proxy_ips;
    if(proxy.type != PROXY_NONE)
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

    std::vector<HostAddr>::const_iterator start, end;

    // do not use proxy for connecting to localhost
    if(proxy.type != PROXY_NONE && hostname != "localhost")
    {
        start = proxy_ips.begin();
        end = proxy_ips.end();
    } else
    {
        start = ips.begin();
        end = ips.end();
    }

    for(auto it = start; it != end; ++it)
    {
        if(it->isUDP)
            throw std::invalid_argument("Cannot connect to UDP (yet)");

        if(!Create(it->ipv6 ? AF_INET6 : AF_INET))
            continue;

        // aktiviere non-blocking
        unsigned long argp = 1;
#ifdef _WIN32
        ioctlsocket(socket_, FIONBIO, &argp);
#else
        ioctl(socket_, FIONBIO, &argp);
#endif

        ResolvedAddr addr(*it);
        if(!addr.isValid())
        {
            LOG.write("Could not resolve %s. Skipping...\n") % it->host;
            continue;
        }
        std::string ip = IpToString(addr.getAddr().ai_addr); //-V807
        LOG.write("Connection to %s%s:%d\n") % (proxy.type != PROXY_NONE ? "Proxy " : "") % ip
          % (proxy.type != PROXY_NONE ? proxy.port : port);

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
                        case PROXY_SOCKS4:
                        {
                            union
                            {
                                char proxyinit[18];
                                unsigned short proxyInitShort[9];
                            };

                            proxyinit[0] = 4; // socks v4
                            proxyinit[1] = 1; // 1=connect
                            proxyInitShort[1] = htons(port);
                            for(std::vector<HostAddr>::const_iterator it = ips.begin(); it != ips.end(); ++it)
                            {
                                if(!it->ipv6)
                                    sscanf(it->host.c_str(), "%c.%c.%c.%c", &proxyinit[4], &proxyinit[5], &proxyinit[6], &proxyinit[7]);
                            }
                            strcpy_check(proxyinit, 8, "siedler25"); // userid

                            Send(proxyinit, 18);

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
                                LOG.write("Proxy error: got %d: connection rejected or failed or other error\n") % proxyinit[1];
                                return false;
                            }
                        }
                        break;
                        case PROXY_SOCKS5:
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
int Socket::Recv(void* buffer, const int length, bool block)
{
    if(!isValid())
        return -1;

    // und empfangen
    return recv(socket_, reinterpret_cast<char*>(buffer), length, (block ? 0 : MSG_PEEK));
}

int Socket::Recv(void* const buffer, const int length, PeerAddr& addr)
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
int Socket::Send(const void* const buffer, const int length)
{
    if(!isValid())
        return -1;

    // und verschicken
    return send(socket_, reinterpret_cast<const char*>(buffer), length, 0);
}

int Socket::Send(const void* const buffer, const int length, const PeerAddr& addr)
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
bool Socket::SetSockOpt(int nOptionName, const void* lpOptionValue, int nOptionLen, int nLevel)
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
bool Socket::operator>(const Socket& sock)
{
    return this->socket_ > sock.socket_;
}

/**
 *  prüft auf wartende Bytes.
 *
 *  @return liefert die Menge der wartenden Bytes
 */
int Socket::BytesWaiting()
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
int Socket::BytesWaiting(unsigned* received)
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
std::string Socket::GetPeerIP()
{
    sockaddr_storage peer;
    socklen_t length = sizeof(sockaddr_storage);

    // Remotehost-Adresse holen
    if(getpeername(socket_, (sockaddr*)&peer, &length) == SOCKET_ERROR)
        return "";

    // in Text verwandeln
    return IpToString((sockaddr*)&peer);
}

/**
 *  liefert die IP des Lokalen-Hosts.
 *
 *  @return liefert @p buffer zurück oder @p "" bei Fehler
 */
std::string Socket::GetSockIP()
{
    sockaddr_storage peer;
    socklen_t length = sizeof(sockaddr_storage);

    // Localhost-Adresse holen
    if(getsockname(socket_, (sockaddr*)&peer, &length) == SOCKET_ERROR)
        return "";

    // in Text verwandeln
    return IpToString((sockaddr*)&peer);
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
    static char temp[256];

#ifdef _WIN32
    size_t size;
    if(addr->sa_family == AF_INET)
        size = sizeof(sockaddr_in);
    else
        size = sizeof(sockaddr_in6);

    sockaddr* copy = (sockaddr*)calloc(1, size);
    memcpy(copy, addr, size);

    if(addr->sa_family == AF_INET)
        ((sockaddr_in*)copy)->sin_port = 0;
    else
        ((sockaddr_in6*)copy)->sin6_port = 0;

    DWORD le = GetLastError();
    DWORD templen = sizeof(temp);
    WSAAddressToStringA(copy, static_cast<DWORD>(size), nullptr, temp, &templen);
    SetLastError(le);

    free(copy);
#else
    const void* ip;

    if(addr->sa_family == AF_INET)
    {
        ip = &(((const sockaddr_in*)addr)->sin_addr);
    } else
    {
        ip = &(((const sockaddr_in6*)addr)->sin6_addr);
    }

    inet_ntop(addr->sa_family, ip, temp, sizeof(temp));
#endif

    return temp;
}
