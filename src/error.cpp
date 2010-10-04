// $Id: error.cpp 6582 2010-07-16 11:23:35Z FloSoft $
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "error.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Fatal-Error-Handler.
 *  Gibt "FATAL ERROR: "+Text auf @p stderr aus
 *
 *  @param[in] format printf-Format-String der Fehlermeldung
 *  @param[in] ...    beliebige Parameter der Funktion
 *
 *  @author FloSoft
 *
 */
int fatal_error(const char *format, ...)
{
	va_list list;
	va_start(list, format);
	LOG.lprintf("\n\nFATAL ERROR: ");
	LOG.lvprintf(format, list);
	LOG.lprintf("\n");

	va_end(list);

	return EXIT_FAILURE;
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Error-Handler.
 *  Gibt "ERROR: "+Text auf @p stderr aus
 *
 *  @param[in] format printf-Format-String der Fehlermeldung
 *  @param[in] ...    beliebige Parameter der Funktion
 *
 *  @author FloSoft
 *
 */
int error(const char *format, ...)
{
	va_list list;
	va_start(list, format);
	LOG.lprintf("\n\nERROR: ");
	LOG.lvprintf(format, list);
	LOG.lprintf("\n");

	va_end(list);

	return EXIT_FAILURE;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Warning-Handler.
 *  Gibt "WARNING: "+Text auf @p stderr aus
 *
 *  @param[in] format printf-Format-String der Fehlermeldung
 *  @param[in] ...    beliebige Parameter der Funktion
 *
 *  @author FloSoft
 *  
 */
int warning(const char *format, ...)
{
	va_list list;
	va_start(list, format);
	LOG.lprintf("\n\nWARNING: ");
	LOG.lvprintf(format, list);
	LOG.lprintf("\n");

	va_end(list);

	return EXIT_SUCCESS;
}
