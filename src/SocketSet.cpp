// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
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

#include "libUtilDefines.h" // IWYU pragma: keep
#include "SocketSet.h"
#include "Socket.h"
#include <cstddef>

SocketSet::SocketSet()
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
void SocketSet::Add(Socket& sock)
{
    // Socket holen
    SOCKET& s = sock.GetSocket();
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
int SocketSet::Select(int timeout, int which)
{
    if(highest == INVALID_SOCKET)
        return SOCKET_ERROR;

    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = timeout;

    fd_set *read, *write, *except;
    read = write = except = NULL;

    // unser Set zuweisen
    switch(which)
    {
        case 0: { read = &set;
        }
        break;
        case 1: { write = &set;
        }
        break;
        default:
        case 2: { except = &set;
        }
        break;
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
bool SocketSet::InSet(Socket& sock)
{
    SOCKET& s = sock.GetSocket();

    // Bei ungültigem Socket ists nicht drin!
    if(s == INVALID_SOCKET)
        return false;

    // Ist unser Socket im Set?
    if(FD_ISSET(s, &set))
        return true;

    // nein, nicht drin
    return false;
}
