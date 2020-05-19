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
#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED

#pragma once

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

using socklen_t = int;
#else
#include <netinet/in.h>
#include <sys/socket.h>

using SOCKET = int;
constexpr auto INVALID_SOCKET = -1;
constexpr auto SOCKET_ERROR = -1;

#define closesocket close
struct addrinfo;
#endif // !_WIN32

#include "ProxySettings.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

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
    sockaddr_storage addr;

public:
    /// Uninitilized value!
    PeerAddr() = default; //-V730
    /// Initializes the address to a broadcast on the given port
    PeerAddr(unsigned short port);

    std::string GetIp() const;
    sockaddr* GetAddr();
    const sockaddr* GetAddr() const;
    int GetSize() const { return sizeof(addr); }
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
    static bool Initialize();

    /// räumt die Socket-Bibliothek auf.
    static void Shutdown();

    /// erstellt und initialisiert das Socket.
    bool Create(int family = AF_INET, bool asUDPBroadcast = false);

    /// schliesst das Socket.
    void Close();

    /// Binds the socket to a specific port
    bool Bind(unsigned short port, bool useIPv6);

    /// setzt das Socket auf Listen.
    bool Listen(unsigned short port, bool use_ipv6 = false, bool use_upnp = true);

    /// akzeptiert eingehende Verbindungsversuche.
    Socket Accept();

    /// versucht eine Verbindung mit einem externen Host aufzubauen.
    bool Connect(const std::string& hostname, unsigned short port, bool use_ipv6, const ProxySettings& proxy = ProxySettings());

    /// liest Daten vom Socket in einen Puffer.
    int Recv(void* buffer, int length, bool block = true);
    /// Reads data from socket and returns peer address. Can be used for unbound sockets
    int Recv(void* buffer, int length, PeerAddr& addr);
    template<typename T, size_t T_size>
    int Recv(std::array<T, T_size>& buffer, const int length = T_size * sizeof(T), bool block = true)
    {
        return Recv(buffer.data(), length, block);
    }

    /// schreibt Daten von einem Puffer auf das Socket.
    int Send(const void* buffer, int length);
    /// Sends data to the specified address (only for connectionless sockets!)
    int Send(const void* buffer, int length, const PeerAddr& addr);
    template<typename T, size_t T_size>
    int Send(const std::array<T, T_size>& buffer)
    {
        return Send(buffer.data(), buffer.size() * sizeof(T));
    }

    /// setzt eine Socketoption.
    bool SetSockOpt(int nOptionName, const void* lpOptionValue, int nOptionLen, int nLevel = IPPROTO_TCP);

    /// Größer-Vergleichsoperator.
    bool operator>(const Socket& sock);

    /// prüft auf wartende Bytes.
    int BytesWaiting();

    /// prüft auf wartende Bytes.
    int BytesWaiting(unsigned* received);

    /// liefert die IP des Remote-Hosts.
    std::string GetPeerIP();

    /// liefert die IP des Lokalen-Hosts.
    std::string GetSockIP();

    /// Gets a reference to the Socket.
    SOCKET GetSocket() const;

    static void Sleep(unsigned ms);

    static std::vector<HostAddr> HostToIp(const std::string& hostname, unsigned port, bool get_ipv6, bool useUDP = false);

    /// liefert einen string der übergebenen Ip.
    static std::string IpToString(const sockaddr* addr);

    /// liefert den Status des Sockets.
    bool isValid() const { return status_ != Status::Invalid; }
    /// Returns true, if this is a broadcast socket (only meaningfull if it is valid)
    bool IsBroadcast() const { return isBroadcast; }

    friend void swap(Socket& s1, Socket& s2)
    {
        using std::swap;
        swap(s1.status_, s2.status_);
        swap(s1.socket_, s2.socket_);
        swap(s1.upnpPort_, s2.upnpPort_);
        swap(s1.refCount_, s2.refCount_);
    }

private:
    /// Setzt ein Socket auf übergebene Werte.
    void Set(SOCKET socket, Status status);

    SOCKET socket_; /// Unser Socket
    /// Number of references to the socket, free only on <=0!
    int32_t* refCount_;
    Status status_;
    bool isBroadcast;
    int16_t upnpPort_; /// UPnP opened port or 0
};

#endif // SOCKET_H_INCLUDED
