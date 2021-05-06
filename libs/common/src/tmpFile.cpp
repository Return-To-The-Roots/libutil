// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "tmpFile.h"
#include <boost/filesystem.hpp>
#include <stdexcept>
#ifdef _WIN32
#    include <windows.h>
#else
#    include <unistd.h>
#endif

namespace bnw = boost::nowide;
namespace bfs = boost::filesystem;

namespace { /// Creates and opens a temporary binary file with the given extension
/// file must be a closed file stream and open() will be called on it
/// Returns the filename used or an empty string on error
bfs::path createTempFile(bnw::ofstream& file, const std::string& ext /* = ".tmp"*/)
{
    static const unsigned MAX_TRIES = 50;

    if(file.is_open())
        throw std::runtime_error("Passed in an open file handle to createTempFile");

    // Temp directory (e.g. /tmp)
    bfs::path tmpPath = bfs::temp_directory_path();
    unsigned tries = 0;
    do
    {
        // Create a (hopefully) unique filePath
        bfs::path filePath = tmpPath / bfs::unique_path();
        filePath += ext;
        // If it exists, try again
        if(bfs::exists(filePath))
            continue;
        // Try to open and place cursor at end if it exists (shouldn't be the case but might be...)
        file.open(filePath, std::ios_base::binary | std::ios_base::ate);
        if(!file)
            continue;
        if(file.tellp() > 0)
        {
            // File contains something (e.g. was just created after the exists check)
            file.close();
            continue;
        }
        return filePath;
    } while(++tries < MAX_TRIES);
    // Could not find a file :(
    return "";
}
} // namespace

void unlinkFile(const boost::filesystem::path& filePath)
{
    if(!bfs::exists(filePath))
        return;
#ifdef _WIN32
    // Try to remove it (works for non-open files and files in non-exclusive mode)
    if(DeleteFileW(filePath.c_str()))
        return;
    // Fallback: Delete on reboot
    MoveFileExW(filePath.c_str(), nullptr, MOVEFILE_DELAY_UNTIL_REBOOT);
#else
    unlink(filePath.c_str());
#endif // _WIN32
}

TmpFile::TmpFile(const std::string& ext /*= ".tmp"*/) : filePath(createTempFile(stream, ext)) {}

TmpFile::~TmpFile()
{
    if(!isValid())
        return;
    stream.close();
    unlinkFile(filePath);
}
