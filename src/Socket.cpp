// $Id: Socket.cpp 9359 2014-04-25 15:37:22Z FloSoft $
//
// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "libUtilDefines.h"
#include "Socket.h"
#include "Log.h"
#include "SocketSet.h"

#ifdef _WIN32

#else
    #include <netinet/tcp.h>
    #include <unistd.h>
    #include <sys/ioctl.h>
    #include <errno.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Standardkonstruktor von @p Socket.
 *
 *  @author FloSoft
 */
Socket::Socket(void) : status(INVALID), sock(INVALID_SOCKET)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p Socket.
 *
 *  @param[in] so Socket welches benutzt werden soll
 *  @param[in] st Status der gesetzt werden soll
 *
 *  @author FloSoft
 */
Socket::Socket(const SOCKET so, STATUS st) : status(st), sock(so)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Setzt ein Socket auf übergebene Werte.
 *
 *  @param[in] so Socket welches benutzt werden soll
 *  @param[in] st Status der gesetzt werden soll
 *
 *  @author FloSoft
 */
void Socket::Set(const SOCKET so, STATUS st)
{
    sock = so;
    status = st;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Initialisiert die Socket-Bibliothek.
 *
 *  @return @p true bei Erfolg, @p false bei Fehler
 *
 *  @author FloSoft
 */
bool Socket::Initialize(void)
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

///////////////////////////////////////////////////////////////////////////////
/**
 *  räumt die Socket-Bibliothek auf.
 *
 *  @author FloSoft
 */
void Socket::Shutdown(void)
{
#ifdef _WIN32
    // obligatorische WSACleanup unter Windows
    WSACleanup();
#endif // _WIN32
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  erstellt und initialisiert das Socket.
 *
 *  @return @p true bei Erfolg, @p false bei Fehler
 *
 *  @author FloSoft
 */
bool Socket::Create(int family)
{
    // socket ggf. schließen
    Close();

    // ist unser Socket schon initialisiert?
    if(status == INVALID)
    {
        // nein, dann Neues erzeugen
        sock = socket(family, SOCK_STREAM, 0);

        // Ist es gültig?
        status = (INVALID_SOCKET == sock ? INVALID : VALID);

        // Nagle deaktivieren
        int disable = 1;
        SetSockOpt(TCP_NODELAY, &disable, sizeof(int), IPPROTO_TCP);

        int enable = 1;
        SetSockOpt(SO_REUSEADDR, &enable, sizeof(int), SOL_SOCKET);
    }

    return (status != INVALID);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schliesst das Socket.
 *
 *  @author FloSoft
 */
void Socket::Close(void)
{
    upnp_.ClosePort();

    if(status != INVALID)
    {
        // Socket schliessen
        closesocket(sock);

        // auf ungültig setzen
        Set(INVALID_SOCKET, INVALID);
    }
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt das Socket auf Listen.
 *
 *  @param[in] port Port auf dem "gelauscht" werden soll.
 *
 *  @return @p true bei Erfolg, @p false bei Fehler
 *
 *  @author FloSoft
 */
bool Socket::Listen(unsigned short port, bool use_ipv6, bool use_upnp)
{
    bool ipv6 = use_ipv6;
    bool error = false;
    int versuch = 0;

    // Adresse initialisieren
    size_t size = 0;

    do
    {
        sockaddr_storage addrs;
        memset(&addrs, 0, sizeof(sockaddr_storage));

        if(ipv6)
        {
            Create(AF_INET6);
            size = sizeof(sockaddr_in6);
        }
        else
        {
            Create(AF_INET);
            size = sizeof(sockaddr_in);
        }

        if(ipv6)
        {
            sockaddr_in6* addr = (sockaddr_in6*)&addrs;
            addr->sin6_family  = AF_INET6;
            addr->sin6_port    = htons(port);
        }
        else
        {
            sockaddr_in* addr = (sockaddr_in*)&addrs;
            addr->sin_family   = AF_INET;
            addr->sin_port     = htons(port);
        }

        // Bei Fehler jeweils nochmal mit ipv4 probieren
        if(status != VALID)
        {
            ipv6 = !ipv6;
            error = true;
            ++versuch;
            if(versuch > 1)
                return false;

            continue;
        }

        // Binden
        if(bind(sock, (sockaddr*)&addrs, size) == SOCKET_ERROR)
        {
            ipv6 = !ipv6;
            error = true;
            ++versuch;
            if(versuch > 1)
                return false;

            continue;
        }

        // und Horchen
        if(listen(sock, 10))
        {
            ipv6 = !ipv6;
            error = true;
            ++versuch;
            if(versuch > 1)
                return false;

            continue;
        }

        error = false;
    }
    while(error);

    // try to open portforwarding if we're using ipv4
    if (use_upnp)
        if(!ipv6 && !upnp_.OpenPort(port))
            LOG.getlasterror("Automatisches Erstellen des Portforwardings mit UPnP fehlgeschlagen\nFehler");

    // Status setzen
    status = LISTEN;

    // und Alles gut :-)
    return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  akzeptiert eingehende Verbindungsversuche.
 *
 *  @param[in,out] client Referenz auf Clientsocket der neuen Verbindung
 *
 *  @return @p true bei Erfolg, @p false bei Fehler
 *
 *  @author FloSoft
 */
bool Socket::Accept(Socket& client)
{
    if(status != LISTEN)
        return false;

    // Verbindung annehmen
    SOCKET tmp = accept(sock, 0, 0);
    if(tmp == INVALID_SOCKET)
        return false;

    // Nagle deaktivieren
    int disable = 1;
    setsockopt(tmp, IPPROTO_TCP, TCP_NODELAY, (char*)&disable, sizeof(int));

    // Status setzen
    client.Set(tmp, CONNECT);

    // Alles gut :-)
    return true;
}


///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert Ip-Adresse(n) für einen Hostnamen.
 *
 *  @param[in] hostname Ziel-Hostname/-ip
 *
 *  @author FloSoft
 */
std::vector<Socket::HostAddr> Socket::HostToIp(const std::string& hostname, const unsigned int port, bool get_ipv6)
{
    std::vector<HostAddr> ips;
    char dport[256];
    snprintf(dport, 255, "%d", port);

    // no dns resolution for localhost
    if(hostname == "localhost")
    {
        HostAddr h;
        h.host = "localhost";
        h.port = dport;
        h.ipv6 = get_ipv6;
        ips.push_back(h);

        return ips;
    }

    addrinfo hints;
    memset(&hints, 0, sizeof(addrinfo));

    hints.ai_flags = AI_ADDRCONFIG | AI_ALL;
    hints.ai_socktype = SOCK_STREAM;

    if(get_ipv6)
        hints.ai_family = AF_INET6;
    else
        hints.ai_family = AF_INET;

    addrinfo* res;
    if(getaddrinfo(hostname.c_str(), dport, &hints, &res) != 0)
        return ips; // "DNS Error"

    addrinfo* addr = res;
    while(addr != NULL)
    {
        HostAddr h;

        h.port = dport;

        if(addr->ai_family == AF_INET6)
            h.ipv6 = true;

        IpToString(addr->ai_addr, h.host);

        addr = addr->ai_next;

        ips.push_back(h);
    }

    freeaddrinfo(res);

    return ips;
}

void Socket::Sleep(unsigned int ms)
{
#ifdef _WIN32
    ::Sleep(250);
#else
    usleep(ms * 1000);
#endif
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  versucht eine Verbindung mit einem externen Host aufzubauen.
 *
 *  @param[in] hostname Ziel-Hostname/-ip
 *  @param[in] port     Port zu dem Verbunden werden soll
 *
 *  @p true bei Erfolg, @p false bei Fehler
 *
 *  @author FloSoft
 */
bool Socket::Connect(const std::string& hostname, const unsigned short port, bool use_ipv6, const Socket::PROXY_TYPE typ, const std::string proxy_hostname, const unsigned int proxy_port)
{
    if(typ == PROXY_SOCKS4)
        use_ipv6 = false;

    std::vector<HostAddr> proxy_ips;
    if(typ != PROXY_NONE)
    {
        proxy_ips = HostToIp(std::string(proxy_hostname), proxy_port, use_ipv6);
        if(proxy_ips.size() == 0)
            return false;
    }

    // TODO: socks v5 kann remote resolven
    std::vector<HostAddr> ips = HostToIp(std::string(hostname), port, use_ipv6);
    if(ips.size() == 0)
        return false;

    bool done = false;

    std::vector<HostAddr>::const_iterator start, end;

    // do not use proxy for connecting to localhost
    if(typ != PROXY_NONE && hostname != "localhost")
    {
        start = proxy_ips.begin();
        end = proxy_ips.end();
    }
    else
    {
        start = ips.begin();
        end = ips.end();
    }

    for(std::vector<HostAddr>::const_iterator it = start; it != end; ++it)
    {
        if(!Create(it->ipv6 ? AF_INET6 : AF_INET))
            continue;

        // aktiviere non-blocking
        unsigned long argp = 1;
#ifdef _WIN32
        ioctlsocket(sock, FIONBIO, &argp);
#else
        ioctl(sock, FIONBIO, &argp);
#endif

        std::string ip;
        IpToString(it->addr->ai_addr, ip);
        LOG.lprintf("Verbinde mit %s%s:%d\n", (typ != PROXY_NONE ? "Proxy " : ""), ip.c_str(), (typ != PROXY_NONE ? proxy_port : port));

        // Und schließlich Verbinden
        if(connect(sock, it->addr->ai_addr, it->addr->ai_addrlen) != SOCKET_ERROR)
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
                    unsigned int err;
                    socklen_t len = sizeof(unsigned int);
                    getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*)&err, &len);

                    if(err != 0)
                    {
#ifdef _WIN32
                        WSASetLastError(err);
#else
                        errno = err;
#endif
                        break;
                    }

                    switch(typ)
                    {
                        default:
                            break;
                        case PROXY_SOCKS4:
                        {
                            char proxyinit[18];

                            proxyinit[0] = 4; // socks v4
                            proxyinit[1] = 1; // 1=connect
                            *reinterpret_cast<unsigned short*>(&proxyinit[2]) = htons(port);
                            for(std::vector<HostAddr>::const_iterator it = ips.begin(); it != ips.end(); ++it)
                            {
                                if(!it->ipv6)
                                    sscanf(it->host.c_str(), "%c.%c.%c.%c", &proxyinit[4], &proxyinit[5], &proxyinit[6], &proxyinit[7]);
                            }
                            strcpy(&proxyinit[8], "siedler25"); // userid

                            Send(proxyinit, 18);

                            int proxy_timeout = 0;
                            while(BytesWaiting() < 8 && proxy_timeout < 8)
                            {
                                Sleep(250);
                                ++proxy_timeout;
                            }

                            if(proxy_timeout >= 8)
                            {
                                LOG.lprintf("Proxy error: connection timed out\n");
                                return false;
                            }

                            Recv(proxyinit, 8);

                            if(proxyinit[0] != 0 || proxyinit[1] != 90)
                            {
                                LOG.lprintf("Proxy error: got %d: connection rejected or failed or other error\n", proxyinit[1]);
                                return false;
                            }
                        } break;
                        case PROXY_SOCKS5:
                        {
                            // not implemented
                            return false;
                        } break;
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
        LOG.getlasterror("Verbindung fehlgeschlagen\nFehler");
    }

    if(!done)
    {
        LOG.lprintf("Fehler beim Verbinden mit %s:%d\n", hostname.c_str(), port);
        return false;
    }
    LOG.lprintf("Verbindung erfolgreich hergestellt mit %s:%d\n", hostname.c_str(), port);

    // deaktiviere non-blocking
    unsigned long argp = 0;
#ifdef _WIN32
    ioctlsocket(sock, FIONBIO, &argp);
#else
    ioctl(sock, FIONBIO, &argp);
#endif

    status = CONNECT;

    // Alles ok
    return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liest Daten vom Socket in einen Puffer.
 *
 *  @param[in] buffer Zielpuffer in den gelesen werden soll
 *  @param[in] length Anzahl an Bytes die gelesen werden soll
 *  @param[in] block  Soll der Vorgang blockierend sein?
 *
 *  @return -1 bei Fehler, Anzahl der empfangenen Bytes (max. @p length )
 *
 *  @author OLiver
 *  @author FloSoft
 */
int Socket::Recv(void* buffer, int length, bool block)
{
    if(status == INVALID)
        return -1;

    // und empfangen
    return recv(sock, reinterpret_cast<char*>(buffer), length, (block ? 0 : MSG_PEEK) );
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schreibt Daten von einem Puffer auf das Socket.
 *
 *  @param[in] buffer Quellpuffer aus dem gelesen werden soll
 *  @param[in] length Anzahl an Bytes die geschrieben werden soll
 *
 *  @return -1 bei Fehler, Anzahl der gesendeten Bytes (max. @p length )
 *
 *  @author OLiver
 *  @author FloSoft
 */
int Socket::Send(const void* buffer, int length)
{
    if(status == INVALID)
        return -1;

    // und verschicken
    return send(sock, reinterpret_cast<const char*>(buffer), length, 0);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt eine Socketoption.
 *
 *  @param[in] buffer Quellpuffer aus dem gelesen werden soll
 *  @param[in] length Anzahl an Bytes die geschrieben werden soll
 *
 *  @return @p true bei Erfolg, @p false bei Fehler
 *
 *  @author OLiver
 *  @author FloSoft
 */
bool Socket::SetSockOpt(int nOptionName, const void* lpOptionValue, int nOptionLen, int nLevel)
{
    return (SOCKET_ERROR != setsockopt(sock, nLevel, nOptionName, (char*)lpOptionValue, nOptionLen));
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Zuweisungsoperator.
 *
 *  @param[in] sock Quellsocket von dem zugewiesen werden soll
 *
 *  @return liefert eine Referenz auf @p this zurück
 *
 *  @author OLiver
 *  @author FloSoft
 */
Socket& Socket::operator =(const Socket& sock)
{
    // Daten setzen
    Set(sock.sock, sock.status);

    upnp_ = sock.upnp_;

    return *this;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Zuweisungsoperator.
 *
 *  @param[in] sock Quellsocket von dem zugewiesen werden soll
 *
 *  @return liefert eine Referenz auf @p this zurück
 *
 *  @author OLiver
 *  @author FloSoft
 */
Socket& Socket::operator =(const SOCKET& sock)
{
    // Daten setzen
    this->sock = sock;
    status = (INVALID_SOCKET == sock ? INVALID : VALID);

    return *this;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Größer-Vergleichsoperator.
 *
 *  @param[in] sock Socket mit dem auf größer verglichen werden soll
 *
 *  @return liefert true falls @p this größer ist als @p sock
 *
 *  @author OLiver
 *  @author FloSoft
 */
bool Socket::operator >(const Socket& sock)
{
    if(this->sock > sock.sock)
        return true;

    return false;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  prüft auf wartende Bytes.
 *
 *  @return liefert die Menge der wartenden Bytes
 *
 *  @author OLiver
 *  @author FloSoft
 */
int Socket::BytesWaiting(void)
{
    unsigned int received;
    if(BytesWaiting(&received) != 0)
        return -1;

    return (int)received;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  prüft auf wartende Bytes.
 *
 *  @param[in,out] dwReceived Menge der wartenden Bytes
 *
 *  @return liefert Null bei Erfolg, SOCKET_ERROR bei Fehler
 *
 *  @author OLiver
 *  @author FloSoft
 */
int Socket::BytesWaiting(unsigned int* received)
{
#ifdef _WIN32
    DWORD dwReceived;
    int retval = ioctlsocket(sock, FIONREAD, &dwReceived);
    *received = dwReceived;
    return retval;
#else
    return ioctl(sock, FIONREAD, received);
#endif
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert die IP des Remote-Hosts.
 *
 *  @return liefert @p buffer zurück oder @p "" bei Fehler
 *
 *  @author OLiver
 *  @author FloSoft
 */
std::string Socket::GetPeerIP(void)
{
    std::string ip;
    sockaddr_storage peer;
    socklen_t length = sizeof(sockaddr_storage);

    // Remotehost-Adresse holen
    if(getpeername(sock, (sockaddr*)&peer, &length) == SOCKET_ERROR)
        return ip;

    // in Text verwandeln
    return IpToString((sockaddr*)&peer, ip);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert die IP des Lokalen-Hosts.
 *
 *  @return liefert @p buffer zurück oder @p "" bei Fehler
 *
 *  @author OLiver
 *  @author FloSoft
 */
std::string Socket::GetSockIP(void)
{
    std::string ip;
    sockaddr_storage peer;
    socklen_t length = sizeof(sockaddr_storage);

    // Localhost-Adresse holen
    if(getsockname(sock, (sockaddr*)&peer, &length) == SOCKET_ERROR)
        return ip;

    // in Text verwandeln
    return IpToString((sockaddr*)&peer, ip);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert einen Zeiger auf das Socket.
 *
 *  @return liefert die Adresse von @p sock zurück
 *
 *  @author OLiver
 *  @author FloSoft
 */
SOCKET* Socket::GetSocket(void)
{
    // Zeiger auf Socket liefern
    return &sock;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert einen string der übergebenen Ip.
 *
 *  @author OLiver
 *  @author FloSoft
 */
std::string& Socket::IpToString(const sockaddr* addr, std::string& buffer)
{
    static char temp[256];

#ifdef _WIN32
    size_t size = 0;
    if (addr->sa_family == AF_INET)
        size = sizeof(sockaddr_in);
    else
        size = sizeof(sockaddr_in6);

    assert(size != 0);

    sockaddr* copy = (sockaddr*)calloc(1, size);
    memcpy(copy, addr, size);

    if (addr->sa_family == AF_INET)
        ((sockaddr_in*)copy)->sin_port = 0;
    else
        ((sockaddr_in6*)copy)->sin6_port = 0;

    DWORD le = GetLastError();
    DWORD templen = sizeof(temp);
    WSAAddressToStringA(copy, size, NULL, temp, &templen);
    SetLastError(le);

    free(copy);
#else
    void* ip;

    if (addr->sa_family == AF_INET)
    {
        ip = &(((sockaddr_in*)addr)->sin_addr);
    }
    else
    {
        ip = &(((sockaddr_in6*)addr)->sin6_addr);
    }

    inet_ntop(addr->sa_family, ip, temp, sizeof(temp));
#endif

    buffer = temp;

    int pos = buffer.find("::ffff:");
    if(pos != -1)
        buffer.replace(pos, 7, "");

    return buffer;
}
