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
#include "fileFuncs.h"

#include "ucString.h"
#include <boost/filesystem.hpp>
#include <cstdlib>
#ifdef _WIN32
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   include <shlobj.h>
#endif

#if defined(__CYGWIN__) || defined (__MINGW32__)

typedef GUID KNOWNFOLDERID;
#define REFKNOWNFOLDERID const KNOWNFOLDERID &

#define DEFINE_KNOWN_FOLDER(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTERN_C const GUID DECLSPEC_SELECTANY name = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

#define KF_FLAG_CREATE 0x00008000

// {4C5C32FF-BB9D-43b0-B5B4-2D72E54EAAA4}
DEFINE_KNOWN_FOLDER(FOLDERID_SavedGames, 0x4c5c32ff, 0xbb9d, 0x43b0, 0xb5, 0xb4, 0x2d, 0x72, 0xe5, 0x4e, 0xaa, 0xa4);
// {FDD39AD0-238F-46AF-ADB4-6C85480369C7}
DEFINE_KNOWN_FOLDER(FOLDERID_Documents,  0xFDD39AD0, 0x238F, 0x46AF, 0xAD, 0xB4, 0x6C, 0x85, 0x48, 0x03, 0x69, 0xC7);

#endif // __MINGW__

#ifdef _WIN32

typedef HRESULT (WINAPI* LPSHGetKnownFolderPath)(REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath);

static LPSHGetKnownFolderPath gSHGetKnownFolderPath = NULL;
static HINSTANCE gShell32DLLInst = NULL;

/**
 *  Wrapper um SHGetKnownFolderPath, unter Vista und Größer benutzt es das
 *  originale SHGetKnownFolderPath, ansonsten SHGetFolderPath.
 *
 *  @param[in] rfid
 *  @param[in] path
 *
 *  @return liefert den Status zurück (S_OK bei OK)
 */
static HRESULT mySHGetKnownFolderPath(REFKNOWNFOLDERID rfid, std::string& path)
{
    HRESULT retval = S_FALSE;
    LPWSTR ppszPath = NULL;

    if(!gShell32DLLInst)
        gShell32DLLInst = LoadLibraryW(L"Shell32.dll");

    if(gShell32DLLInst && !gSHGetKnownFolderPath)
        gSHGetKnownFolderPath = (LPSHGetKnownFolderPath)GetProcAddress(gShell32DLLInst, "SHGetKnownFolderPath");

    if(gSHGetKnownFolderPath)
        retval = gSHGetKnownFolderPath(rfid, KF_FLAG_CREATE, NULL, &ppszPath);
    else if(rfid == FOLDERID_Documents)
    {
        ppszPath = (LPWSTR)CoTaskMemAlloc(MAX_PATH * sizeof(WCHAR));
        if(SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, NULL, 0, ppszPath)))
            retval = S_OK;
    }

    if(ppszPath)
    {
        path = cvWideStringToUTF8(ppszPath);
        CoTaskMemFree(ppszPath);
    }

    return retval;
}
#endif


/**
 *  formt Pfade korrekt um.
 *
 *  @param[in] file
 *
 *  @return liefert den umgeformten Pfad zurück
 */
std::string GetFilePath(const std::string& file)
{
    if(file.empty())
        return "";

    bfs::path to;

    // ist der Pfad ein Home-Dir?
    if(file[0] == '~')
    {
        std::string homePath;

#ifdef _WIN32
        // "$User\Saved Games"
        if(mySHGetKnownFolderPath(FOLDERID_SavedGames, homePath) != S_OK)
        {
            // "$Documents\My Games"
            if(mySHGetKnownFolderPath(FOLDERID_Documents, homePath) == S_OK)
                homePath += "\\My Games";
        }

        // Kein Pfad gefunden, $AppData verwenden
        if(homePath.empty())
            homePath = getenv("APPDATA");

// linux, apple
#else
        // $Home verwenden
        homePath = getenv("HOME");
#endif
        // Emergency fallback. Should never happen but prevents "~/foo"->"/foo"
        if(homePath.empty())
        {
            assert(false);
            homePath = ".";
        }

        to = homePath + file.substr(1);
    } else
        to = file;

    return to.make_preferred().string();
}
