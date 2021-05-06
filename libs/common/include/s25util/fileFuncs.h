// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>

/// Remove all invalid chars of a file or directory name. Result may be empty!
/// --> bfs::portable_name will return true
std::string makePortableName(const std::string& fileName);
/// Remove all invalid chars so the name can be used for a file. Result may be empty!
/// --> bfs::portable_file_name will return true
std::string makePortableFileName(const std::string& fileName);
/// Remove all invalid chars so the name can be used for a directory. Result may be empty!
/// --> bfs::portable_directory_name will return true
std::string makePortableDirName(const std::string& fileName);
