// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SocketSet.h"
#include "Socket.h"

SocketSet::SocketSet() //-V730
{
    Clear();
}

/**
 *  räumt das @p SocketSet auf.
 */
void SocketSet::Clear()
{
    FD_ZERO(&set);

    highest = INVALID_SOCKET;
}

/**
 *  fügt ein @p Socket zum @p SocketSet hinzu.
 *
 *  @param[in] sock Socket welches hinzugefügt werden soll
 */
void SocketSet::Add(const Socket& sock)
{
    // Socket holen
    SOCKET s = sock.GetSocket();
    if(s == INVALID_SOCKET)
        return;

    // hinzufügen
    FD_SET(s, &set);

    // neues größtes Socket setzen
    if(s > highest || highest == INVALID_SOCKET)
        highest = s;
}

/**
 *  führt einen Select auf dem @p SocketSet aus.
 *
 *  @param[in] wait  Timeout des Aufrufs
 *  @param[in] which Welcher der "Kanäle" soll abgefragt werden? (0 = read, 1 = write, 2 = error)
 *
 *  @return liefert SOCKET_ERROR bei Fehler, Null bei Timeout, größer Null für die Anzahl der bereiten Sockets im Set
 */
int SocketSet::Select(unsigned timeoutInMs, int which)
{
    if(highest == INVALID_SOCKET)
        return SOCKET_ERROR;

    timeval tv;
    tv.tv_sec = timeoutInMs / 1000u;
    tv.tv_usec = (timeoutInMs - tv.tv_sec * 1000) * 1000;

    fd_set *read, *write, *except;
    read = write = except = nullptr;

    // unser Set zuweisen
    switch(which)
    {
        case 0: read = &set; break;
        case 1: write = &set; break;
        default:
        case 2: except = &set; break;
    }

    // Select ausführen
    return select((int)highest + 1, read, write, except, &tv);
}

/**
 *  ist ein @p Socket im @p SocketSet ?
 *
 *  @param[in] sock Socket welches im Set gesucht werden soll
 *
 *  @return liefert true falls das @p Socket im Set enthalten ist, false andernfalls
 */
bool SocketSet::InSet(const Socket& sock)
{
    SOCKET s = sock.GetSocket();

    // Bei ungültigem Socket ists nicht drin!
    if(s == INVALID_SOCKET)
        return false;

    return FD_ISSET(s, &set) != 0;
}
