// Copyright (C) 2005 - 2026 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>

/// Sanitizes to a portable name. Appends '_' to Windows reserved device names. Result may be empty.
/// --> bfs::portable_name will return true for non-empty results
std::string makePortableName(const std::string& fileName);
/// Sanitizes to a portable filename. Result may be empty.
/// --> bfs::portable_file_name will return true for non-empty results
std::string makePortableFileName(const std::string& fileName);
/// Sanitizes to a portable directory name. Result may be empty.
/// --> bfs::portable_directory_name will return true for non-empty results
std::string makePortableDirName(const std::string& fileName);

/// Returns true if c is valid in a user-provided filename.
/// Rejects control characters and chars forbidden on Windows (< > : " / \ | ? *).
bool isValidFileNameChar(char32_t c);
/// Returns true if fileName is a valid user-provided filename.
/// Rejects reserved device names, empty names, leading/trailing dots, and trailing spaces.
bool isValidFileName(const std::string& fileName);
