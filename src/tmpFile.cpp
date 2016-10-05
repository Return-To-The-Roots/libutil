// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
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

#include "libUtilDefines.h" // IWYU pragma: keep
#include "tmpFile.h"

#include <boost/filesystem.hpp>
#include <fstream>
#include <stdexcept>
#include <iostream>
#ifdef WIN32
#   include <windows.h>
#else
#   include <unistd.h>
#endif

namespace bfs = boost::filesystem;

std::string createTempFile(std::ofstream& file, const std::string& ext/* = ".tmp"*/)
{
    static const unsigned MAX_TRIES = 50;

    if(file.is_open())
        throw std::runtime_error("Passed in an open file handle tp createTempFile");

    // Temp directory (e.g. /tmp)
    bfs::path tmpPath = bfs::temp_directory_path();
    unsigned tries = 0;
    do 
    {
        // Create a (hopefully) unique filePath
        bfs::path filePath = tmpPath;
        filePath /= bfs::unique_path();
        filePath += ext;
        // If it exists, try again
        if(bfs::exists(filePath))
            continue;
        // Try to open and place cursor at end if it exists (shouldn't be the case but might be...)
        file.open(filePath.c_str(), std::ios_base::binary | std::ios_base::ate);
        if(!file)
            continue;
        if(file.tellp() > 0)
        {
            // File contains something (e.g. was just created after the exists check)
            file.close();
            continue;
        }
        return filePath.string();
    } while(++tries < MAX_TRIES);
    // Could not find a file :(
    return "";
}

void unlinkFile(const std::string& filePath)
{
    bfs::path filepathFull = filePath;
    if(!bfs::exists(filepathFull))
        return;
#ifdef WIN32
    // Try to remove it (works for non-open files and files in non-exclusive mode)
    if(DeleteFileW(filepathFull.c_str()))
        return;
    // Fallback: Delete on reboot
    MoveFileExW(filepathFull.c_str(), NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
#else
    unlink(filepathFull.c_str());
#endif // WIN32

}

TmpFile::TmpFile(const std::string& ext /*= ".tmp"*/):
    filePath(createTempFile(stream, ext))
{}

TmpFile::~TmpFile()
{
    if(!IsValid())
        return;
    stream.close();
    unlinkFile(filePath);
}
