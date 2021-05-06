// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <string>

// Provides safe C-String functions by preventing buffer overflows
// Better use std::string and boost::format!

namespace detail {

/// Copies a C-String into a buffer (with terminating zero)
/// Throws an exception on buffer overflow (strlen(pSrc) >= maxChars)
void strcpyExt(char* pOut, const char* pSrc, size_t maxChars);
} // namespace detail

/// Copies a C-String into a buffer (with terminating zero)
/// Throws an exception on buffer overflow (strlen(pSrc) >= T_size)
template<size_t T_size>
void strcpy_check(std::array<char, T_size>& output, const size_t offset, const char* pSrc)
{
    assert(offset <= T_size);
    detail::strcpyExt(output.data() + offset, pSrc, T_size - offset);
}

/// Create a random string of length len using the given categories
std::string createRandString(size_t len, bool useLowercase = true, bool useUppercase = true, bool useNumbers = true,
                             bool useSpecialChars = true);
/// Create a random string with chars from the given charset
std::string createRandString(size_t len, const std::string& charset);
/// Create a random string with chars from the given charset and seed
std::string createRandString(size_t len, const std::string& charset, uint32_t seed);
