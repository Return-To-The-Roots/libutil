// Copyright (c) 2016 - 2017 Settlers Freaks (sf-team at siedler25.org)
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

#include "System.h"
#include <boost/nowide/convert.hpp>
#include <windows.h>
#include <shellapi.h>
#ifdef _MSC_VER
#    pragma warning(push)
#    pragma warning(disable : 4091) // "typedef ": Ignoriert auf der linken Seite von "tagGPFIDL_FLAGS"
#endif
#include <shlobj.h>
#ifdef _MSC_VER
#    pragma warning(push)
#endif
#include <stdexcept>

namespace bfs = boost::filesystem;

#if defined(__MINGW32__) && !defined(REFKNOWNFOLDERID)

using KNOWNFOLDERID = GUID;
#    define REFKNOWNFOLDERID const KNOWNFOLDERID&

#    define DEFINE_KNOWN_FOLDER(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTERN_C const GUID DECLSPEC_SELECTANY name = {l, w1, w2, {b1, b2, b3, b4, b5, b6, b7, b8}}

#    define KF_FLAG_CREATE 0x00008000

// {4C5C32FF-BB9D-43b0-B5B4-2D72E54EAAA4}
DEFINE_KNOWN_FOLDER(FOLDERID_SavedGames, 0x4c5c32ff, 0xbb9d, 0x43b0, 0xb5, 0xb4, 0x2d, 0x72, 0xe5, 0x4e, 0xaa, 0xa4);
// {FDD39AD0-238F-46AF-ADB4-6C85480369C7}
DEFINE_KNOWN_FOLDER(FOLDERID_Documents, 0xFDD39AD0, 0x238F, 0x46AF, 0xAD, 0xB4, 0x6C, 0x85, 0x48, 0x03, 0x69, 0xC7);

#elif(NTDDI_VERSION < NTDDI_VISTA)
#    define KF_FLAG_CREATE 0x00008000
#endif // __MINGW32__

typedef HRESULT(WINAPI* LPSHGetKnownFolderPath)(REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath);

static LPSHGetKnownFolderPath gSHGetKnownFolderPath = nullptr;
static HINSTANCE gShell32DLLInst = nullptr;

/**
 *  Wrapper around SHGetKnownFolderPath, on Vista and up it uses SHGetKnownFolderPath,
 *  else SHGetFolderPath.
 *
 *  @param[in] rfid
 *  @param[in] path
 *
 *  @return Status of operation (S_OK for OK)
 */
static HRESULT mySHGetKnownFolderPath(REFKNOWNFOLDERID rfid, std::string& path)
{
    HRESULT retval = S_FALSE;
    LPWSTR ppszPath = nullptr;

    if(!gShell32DLLInst)
        gShell32DLLInst = LoadLibraryW(L"Shell32.dll");

    if(gShell32DLLInst && !gSHGetKnownFolderPath)
        gSHGetKnownFolderPath = (LPSHGetKnownFolderPath)GetProcAddress(gShell32DLLInst, "SHGetKnownFolderPath");

    if(gSHGetKnownFolderPath)
        retval = gSHGetKnownFolderPath(rfid, KF_FLAG_CREATE, nullptr, &ppszPath);
    else if(rfid == FOLDERID_Documents)
    {
        ppszPath = (LPWSTR)CoTaskMemAlloc(MAX_PATH * sizeof(WCHAR));
        if(ppszPath && SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, nullptr, 0, ppszPath)))
            retval = S_OK;
    }

    if(ppszPath)
    {
        path = boost::nowide::narrow(ppszPath);
        CoTaskMemFree(ppszPath);
    }

    return retval;
}

bfs::path System::getHomePath()
{
    bfs::path homePath;

    // "$User\Saved Games"
    std::string tmpPath;
    if(mySHGetKnownFolderPath(FOLDERID_SavedGames, tmpPath) == S_OK)
        homePath = tmpPath;
    else
    {
        // "$Documents\My Games"
        if(mySHGetKnownFolderPath(FOLDERID_Documents, tmpPath) == S_OK)
            homePath = bfs::path(tmpPath) / "My Games";
    }

    // No path found, use $AppData
    if(homePath.empty())
        homePath = getPathFromEnvVar("APPDATA");

    // Emergency fallback. Should never happen but prevents "~/foo"->"/foo"
    if(homePath.empty())
    {
        assert(false);
        homePath = ".";
    }
    return homePath;
}

std::string System::getUserName()
{
    DWORD nameLen = 0;
    // Query required size
    GetUserNameW(nullptr, &nameLen);
    if(nameLen == 0)
        throw std::runtime_error("Could not query username length");
    std::vector<wchar_t> userName(nameLen);
    if(GetUserNameW(userName.data(), &nameLen) == 0)
        throw std::runtime_error("Could not get username");

    userName.resize(nameLen - 1); // nameLen already contains terminating 0

    return boost::nowide::narrow(std::wstring(userName.begin(), userName.end()));
}
