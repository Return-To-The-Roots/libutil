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

#ifndef ucString_h__
#define ucString_h__

#include <stdint.h>
#include <string>

/// Typedef for storing and managing an Unicode (UTF-32) string
typedef std::basic_string<uint32_t> ucString;

//////////////////////////////////////////////////////////////////////////
// Convert UTF8<->Unicode strings

/// Convert an UTF-8 string to unicode string
ucString cvUTF8ToUnicode(const std::string& other);

/// Convert an unicode string to UTF-8 string
std::string cvUnicodeToUTF8(const ucString& other);

/// Return true if text is valid UTF-8
bool isValidUTF8(const std::string& text);

//////////////////////////////////////////////////////////////////////////
// Legacy functions for wide strings (non-portable unicode versions)

/// Convert a widestring (assumed to be UTF-16 or UCS-2) to unicode string
std::string cvWideStringToUTF8(const std::wstring& other);
/// Convert a widestring (assumed to be UTF-16 or UCS-2) to unicode string
ucString cvWideStringToUnicode(const std::wstring& other);
/// Convert an UTF-8 string to a widestring (UTF-16 or UCS-2)
std::wstring cvUTF8ToWideString(const std::string& other);

//////////////////////////////////////////////////////////////////////////
// Functions that interpret the std::string as unicode
// and simply convert 0x80-0xFF to U+80-U+FF
// Note that this can badly fail as the source might be from a different
// locale as the destination

/// Convert a string (assumed to be codepage local) to UTF-8 unless it already is UTF-8
std::string cvStringToUTF8(const std::string& other);
/// Convert a string (assumed to be codepage local) to unicode string
ucString cvStringToUnicode(const std::string& other);

#endif // ucString_h__
