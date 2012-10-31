// $Id: main.h 8476 2012-10-31 15:13:00Z FloSoft $
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
#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#pragma once

///////////////////////////////////////////////////////////////////////////////
// System-Header

#define _CRTDBG_MAP_ALLOC
#define WIN32_LEAN_AND_MEAN

#ifdef _WIN32
	#include <windows.h>
	
	#ifndef __CYGWIN__
		#include <conio.h>
	#endif

	#include <ws2tcpip.h>
	
	#ifdef __CYGWIN__
		#ifndef AI_ALL
		#define AI_ALL            0x0010
		#endif
		
		#ifndef AI_ADDRCONFIG
		#define AI_ADDRCONFIG     0x0020
		#endif
	#endif

#ifdef _MSC_VER
	#include <crtdbg.h>
#else
    #include <assert.h>
#endif
#else
	#include <string.h>
	#include <unistd.h>
	#include <stdarg.h>
	#include <signal.h>
	#include <dirent.h>
	#include <dlfcn.h>
	#include <netdb.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/select.h>
	#include <sys/ioctl.h>
	#include <sys/times.h>
	#include <sys/stat.h>
	#include <sys/times.h>
	#include <arpa/inet.h>
	#include <assert.h>
	#include <errno.h>
#endif // !_WIN32

#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cstring>

#include <algorithm>
#include <string>
#include <sstream>
#include <vector>

#if defined _WIN32 && defined _DEBUG
	#include <crtdbg.h>
#endif // _WIN32 && _DEBUG

#ifdef _WIN32

#ifdef _MSC_VER
	#define getch _getch
	#ifndef snprintf
		#define snprintf _snprintf
	#endif
	#define assert _ASSERT
#endif

#endif // _WIN32

#ifndef va_copy
/* WARNING - DANGER - ASSUMES TYPICAL STACK MACHINE */
#define va_copy(dst, src) ((void)((dst) = (src)))
#endif

///////////////////////////////////////////////////////////////////////////////
// Eigene Header
#include <build_paths.h>
#include "libutil.h"

#endif // MAIN_H_INCLUDED
