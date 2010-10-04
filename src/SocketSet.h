// $Id: SocketSet.h 6582 2010-07-16 11:23:35Z FloSoft $
//
// Copyright (c) 2005 - 2010 Settlers Freaks (sf-team at siedler25.org)
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
#ifndef SOCKETSET_H_INCLUDED
#define SOCKETSET_H_INCLUDED

#pragma once

class Socket;

/// FD_Set-Wrapper-Klasse für portable TCP/IP-Verbindungen
class SocketSet
{
public:
	/// Standardkonstruktor von @p SocketSet.
	SocketSet();

	/// räumt das @p SocketSet auf.
	void Clear(void);

	/// fügt ein @p Socket zum @p SocketSet hinzu.
	void Add(Socket &sock);

	/// führt einen Select auf dem @p SocketSet aus.
	int Select(int timeout, int which = 2);

	/// ist ein @p Socket im @p SocketSet?
	bool InSet(Socket &sock);

private:
	fd_set set;     ///< Das fd_set
	SOCKET highest; ///< Höchste Socket des Sets
};

#endif // SOCKETSET_H_INCLUDED
