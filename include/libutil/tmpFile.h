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

#include <boost/nowide/fstream.hpp>
#include <string>

/// Removes a file
/// If the file is currently in use, it is registered for deletion
/// (either on close or on reboot)
void unlinkFile(const std::string& filePath);

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

    const std::string filePath;
};
