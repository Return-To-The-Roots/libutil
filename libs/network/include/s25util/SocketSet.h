// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#ifdef _WIN32
#    include <winsock2.h>
#else
#    include <sys/select.h>
using SOCKET = int;
#endif // _WIN32

class Socket;

/// FD_Set-Wrapper-Klasse für portable TCP/IP-Verbindungen
class SocketSet
{
public:
    SocketSet();

    /// räumt das @p SocketSet auf.
    void Clear();

    /// fügt ein @p Socket zum @p SocketSet hinzu.
    void Add(const Socket& sock);

    /// führt einen Select auf dem @p SocketSet aus.
    int Select(unsigned timeoutInMs, int which = 2);

    /// ist ein @p Socket im @p SocketSet?
    bool InSet(const Socket& sock);

private:
    fd_set set;     /// Das fd_set
    SOCKET highest; /// Höchste Socket des Sets
};
