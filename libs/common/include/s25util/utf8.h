// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>

namespace s25util {

/// Convert an UTF-8 string to unicode string
std::u32string utf8to32(const std::string& other);

/// Convert an unicode string to UTF-8 string
std::string utf32to8(const std::u32string& other);

/// Return true if text is valid UTF-8
bool isValidUTF8(const std::string& text);
std::string::const_iterator findInvalidUTF8(const std::string& text);

/// Convert ANSI (aka Windows-1252) encoded strings to UTF-8
std::string ansiToUTF8(const std::string& other);

} // namespace s25util
