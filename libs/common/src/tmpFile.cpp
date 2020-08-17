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

#include "tmpFile.h"
#include <boost/filesystem.hpp>
#include <stdexcept>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
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
