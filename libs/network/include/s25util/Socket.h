// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#ifdef _WIN32

#    include <winsock2.h>
#    include <ws2tcpip.h>

using socklen_t = int;
#else
#    include <netinet/in.h>
#    include <sys/socket.h>

using SOCKET = int;
constexpr auto INVALID_SOCKET = -1;
constexpr auto SOCKET_ERROR = -1;

#    define closesocket close
struct addrinfo;
#endif // !_WIN32

#include "ProxySettings.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

/// used for type punning
union address_t
{
    sockaddr sa;
    sockaddr_in sa_in;
    sockaddr_in6 sa_in6;
    sockaddr_storage sa_stor;
};

/// liefert Ip-Adresse(n) für einen Hostnamen.
struct HostAddr
{
    HostAddr() : port("0"), ipv6(false), isUDP(false) {}

    std::string host;
    std::string port;
    bool ipv6, isUDP;
};

/// Resolves a host address
class ResolvedAddr
{
    const bool lookup;
    addrinfo* addr;

public:
    ResolvedAddr(const HostAddr& hostAddr, bool resolveAll = false);
    ResolvedAddr(const ResolvedAddr&) = delete;
    ResolvedAddr& operator=(const ResolvedAddr&) = delete;
    ~ResolvedAddr();

    addrinfo& getAddr() { return *addr; }
    bool isValid() const { return addr != nullptr; }
};

class PeerAddr
{
    address_t addr{};
    socklen_t addrLen = 0;

public:
    /// Empty address with room for any address type, e.g. to be filled by Socket::recv
    PeerAddr() = default;
    /// Initializes the address to a broadcast on the given port
    PeerAddr(unsigned short port);

    std::string getIp() const;
    sockaddr* getAddr();
    const sockaddr* getAddr() const;
    /// Size of the contained address (suitable for sendto)
    socklen_t getSize() const { return addrLen; }
    /// Capacity of the underlying storage (suitable for recvfrom)
    static constexpr socklen_t getCapacity() { return sizeof(address_t); }
    /// set the size of the contained address, e.g. after it has been filled by recvfrom
    void setSize(socklen_t size) { addrLen = size; }
};

/// Socket-Wrapper-Klasse für portable TCP/IP-Verbindungen
class Socket
{
private:
    enum class Status
    {
        Invalid,
        Valid,
        Listen,
        Connected
    };

public:
    Socket();
    Socket(SOCKET so, Status st);
    Socket(const Socket& so);
    Socket(Socket&& so) noexcept;
    ~Socket();

    Socket& operator=(Socket rhs);

    /// Initialisiert die Socket-Bibliothek.
    static bool initialize();

    /// räumt die Socket-Bibliothek auf.
    static void shutdown();

    /// erstellt und initialisiert das Socket.
    bool create(int family = AF_INET, bool asUDPBroadcast = false);

    /// schliesst das Socket.
    void close();

    /// Binds the socket to a specific port
    bool bind(unsigned short port, bool useIPv6) const;

    /// setzt das Socket auf listen.
    bool listen(unsigned short port, bool use_ipv6 = false, bool use_upnp = true);

    /// akzeptiert eingehende Verbindungsversuche.
    Socket accept();

    /// versucht eine Verbindung mit einem externen Host aufzubauen.
    bool connect(const std::string& hostname, unsigned short port, bool use_ipv6,
                 const ProxySettings& proxy = ProxySettings());

    /// liest Daten vom Socket in einen Puffer.
    int recv(void* buffer, int length, bool block = true) const;
    /// Reads data from socket and returns peer address. Can be used for unbound sockets
    int recv(void* buffer, int length, PeerAddr& addr) const;
    template<typename T, size_t T_size>
    int recv(std::array<T, T_size>& buffer, const int length = T_size * sizeof(T), bool block = true)
    {
        return recv(buffer.data(), length, block);
    }

    /// schreibt Daten von einem Puffer auf das Socket.
    int send(const void* buffer, int length) const;
    /// Sends data to the specified address (only for connectionless sockets!)
    int send(const void* buffer, int length, const PeerAddr& addr) const;
    template<typename T, size_t T_size>
    int send(const std::array<T, T_size>& buffer)
    {
        return send(buffer.data(), buffer.size() * sizeof(T));
    }

    /// setzt eine Socketoption.
    bool setSockOpt(int nOptionName, const void* lpOptionValue, int nOptionLen, int nLevel = IPPROTO_TCP) const;

    /// Größer-Vergleichsoperator.
    bool operator>(const Socket& sock) const;

    /// prüft auf wartende Bytes.
    int bytesWaiting() const;

    /// prüft auf wartende Bytes.
    int bytesWaiting(unsigned* received) const;

    /// liefert die IP des Remote-Hosts.
    std::string getPeerIP() const;

    /// liefert die IP des Lokalen-Hosts.
    std::string getSockIP() const;

    /// Gets a reference to the Socket.
    SOCKET getSocket() const;

    static void sleep(unsigned ms);

    static std::vector<HostAddr> hostToIp(const std::string& hostname, unsigned port, bool get_ipv6,
                                          bool useUDP = false);

    /// liefert einen string der übergebenen Ip.
    static std::string ipToString(const sockaddr* addr);

    /// liefert den Status des Sockets.
    bool isValid() const { return status_ != Status::Invalid; }
    /// Returns true, if this is a broadcast socket (only meaningfull if it is valid)
    bool isBroadcast() const { return isBroadcast_; }

    friend void swap(Socket& s1, Socket& s2) noexcept
    {
        using std::swap;
        swap(s1.status_, s2.status_);
        swap(s1.socket_, s2.socket_);
        swap(s1.upnpPort_, s2.upnpPort_);
        swap(s1.refCount_, s2.refCount_);
    }

private:
    /// Setzt ein Socket auf übergebene Werte.
    void set(SOCKET socket, Status status);

    SOCKET socket_; /// Unser Socket
    /// Number of references to the socket, free only on <=0!
    int32_t* refCount_;
    Status status_;
    bool isBroadcast_;
    int16_t upnpPort_; /// UPnP opened port or 0
};
