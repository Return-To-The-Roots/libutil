// Copyright (C) 2005 - 2026 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "fileFuncs.h"
#include "s25util/strAlgos.h"
#include "s25util/utf8.h"
#include <boost/filesystem/path.hpp>
#include <algorithm>
#include <array>

namespace bfs = boost::filesystem;

// Windows reserved device names
static constexpr std::array reservedNames{"con",  "prn",  "aux",  "nul",  "com0", "com1", "com2", "com3",
                                          "com4", "com5", "com6", "com7", "com8", "com9", "lpt0", "lpt1",
                                          "lpt2", "lpt3", "lpt4", "lpt5", "lpt6", "lpt7", "lpt8", "lpt9"};

static bool isReservedName(const std::string& name)
{
    const std::string lower = s25util::toLower(name);
    return std::find(reservedNames.begin(), reservedNames.end(), lower) != reservedNames.end();
}

std::string makePortableName(const std::string& fileName)
{
    if(fileName.empty() || bfs::portable_name(fileName))
        return isReservedName(fileName) ? fileName + '_' : fileName;
    std::string result;
    result.reserve(fileName.size());
    for(char c : fileName)
    {
        // Characters allowed by portable_posix_name. These are also allowed by windows_name
        if((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '.' || c == '_'
           || c == '-')
            result += c;
        else
            result += '_';
    }
    if(result != "." && result != "..")
    {
        // Remove leading and trailing dots
        while(!result.empty() && result.front() == '.')
            result.erase(result.begin());
        while(!result.empty() && result.back() == '.')
            result.erase(result.end() - 1);
    }
    if(isReservedName(result))
        result += '_';
    assert(result.empty() || bfs::portable_name(result));
    return result;
}

std::string makePortableFileName(const std::string& fileName)
{
    std::string result = makePortableName(fileName);
    if(result.empty() || bfs::portable_file_name(result))
        return result;
    // Remove all dots but the last
    size_t pos = result.find('.');
    if(pos != std::string::npos)
    {
        size_t nextPos;
        while((nextPos = result.find('.', pos + 1)) != std::string::npos)
        {
            result.erase(pos, 1u);
            pos = nextPos - 1u;
        }
        // Only 3 additional chars after the dot
        if(pos + 4u < result.size())
            result.resize(pos + 4u);
    }
    if(result == "." || result == "..")
        result.clear();
    assert(result.empty() || bfs::portable_file_name(result));
    return result;
}

std::string makePortableDirName(const std::string& fileName)
{
    std::string result = makePortableName(fileName);
    if(result.empty() || bfs::portable_directory_name(result))
        return result;
    // Remove all dots unless we are the special DOT or DOTDOT
    if(result != "." && result != "..")
    {
        size_t pos;
        while((pos = result.find('.')) != std::string::npos)
            result.erase(pos, 1u);
    }
    assert(result.empty() || bfs::portable_directory_name(result));
    return result;
}

bool isValidFileNameChar(char32_t c)
{
    // Reject control characters
    if(c <= 0x1F || c == 0x7F)
        return false;
    // Reject characters forbidden on Windows (the most restrictive platform),
    // which covers all restrictions on Linux, macOS, and Android as well.
    if(c == '<' || c == '>' || c == ':' || c == '"' || c == '/' || c == '\\' || c == '|' || c == '?' || c == '*')
        return false;
    return true;
}

bool isValidFileName(const std::string& fileName)
{
    if(fileName.empty())
        return false;
    const auto asU32 = s25util::utf8to32(fileName);
    if(asU32.size() > 255)
        return false;
    if(asU32.front() == U'.' || asU32.back() == U'.')
        return false;
    // Windows silently strips trailing spaces, which would create a mismatch between
    // the name the user typed and the file actually created on disk.
    if(asU32.back() == U' ')
        return false;
    for(char32_t c : asU32)
    {
        if(!isValidFileNameChar(c))
            return false;
    }
    // On Windows 7 and earlier the device name is the part before the first dot — "nul.ini" is NUL thus forbidden.
    return !isReservedName(fileName.substr(0, fileName.find('.')));
}
