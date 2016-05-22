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

#include <fstream>
#include <string>

/// Creates and opens a temporary binary file with the given extension
/// file must be a closed file stream and open() will be called on it
/// Returns the filename used or an empty string on error
std::string createTempFile(std::ofstream& file, const std::string& ext = ".tmp");

/// Removes a file
/// If the file is currently in use, it is registered for deletion
/// (either on close or on reboot)
void unlinkFile(const std::string& filePath);

/// RAII wrapper for a temporary file, that creates and opens it and deletes it on destruction
class TmpFile
{
    std::ofstream stream;
public:
    explicit TmpFile(const std::string& ext = ".tmp");
    ~TmpFile();

    bool IsValid() const { return !filePath.empty(); }
    std::ofstream& GetStream(){ return stream; }
    const std::ofstream& GetStream() const { return stream; }

    const std::string filePath;
};
