// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <boost/filesystem/path.hpp>
#include <boost/nowide/fstream.hpp>
#include <string>

/// Removes a file
/// If the file is currently in use, it is registered for deletion
/// (either on close or on reboot)
void unlinkFile(const boost::filesystem::path& filePath);

/// RAII wrapper for a temporary file, that creates and opens it and deletes it on destruction
class TmpFile
{
    boost::nowide::ofstream stream;

public:
    explicit TmpFile(const std::string& ext = ".tmp");
    ~TmpFile();

    bool isValid() const { return !filePath.empty(); }
    std::ostream& getStream() { return stream; }
    const std::ostream& getStream() const { return stream; }
    void close() { stream.close(); }

    const boost::filesystem::path filePath;
};
