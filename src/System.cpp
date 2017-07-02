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

#include "libUtilDefines.h" // IWYU pragma: keep
#include "System.h"
#include "ucString.h"
#include "getExecutablePath.h"
#include <cstdlib>
#ifdef _WIN32
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   include <shlobj.h>
#   include <shellapi.h>
#   include <stdexcept>
#endif // _WIN32

#if defined(__CYGWIN__) || defined (__MINGW32__)

typedef GUID KNOWNFOLDERID;
#define REFKNOWNFOLDERID const KNOWNFOLDERID &

#define DEFINE_KNOWN_FOLDER(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTERN_C const GUID DECLSPEC_SELECTANY name = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

#define KF_FLAG_CREATE 0x00008000

// {4C5C32FF-BB9D-43b0-B5B4-2D72E54EAAA4}
DEFINE_KNOWN_FOLDER(FOLDERID_SavedGames, 0x4c5c32ff, 0xbb9d, 0x43b0, 0xb5, 0xb4, 0x2d, 0x72, 0xe5, 0x4e, 0xaa, 0xa4);
// {FDD39AD0-238F-46AF-ADB4-6C85480369C7}
DEFINE_KNOWN_FOLDER(FOLDERID_Documents, 0xFDD39AD0, 0x238F, 0x46AF, 0xAD, 0xB4, 0x6C, 0x85, 0x48, 0x03, 0x69, 0xC7);

#endif // __MINGW__

#ifdef _WIN32

typedef HRESULT(WINAPI* LPSHGetKnownFolderPath)(REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath);

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

//////////////////////////////////////////////////////////////////////////

std::string System::getEnvVar(const std::string& name){
#ifdef _WIN32
    return cvWideStringToUTF8(getEnvVar(cvUTF8ToWideString(name)));
#else
    const char* value = getenv(name.c_str());
    return value ? value : "";
#endif // _WIN32
}

std::wstring System::getEnvVar(const std::wstring& name){
#ifdef _WIN32
    DWORD numBytes = GetEnvironmentVariableW(name.c_str(), NULL, 0);
    if(!numBytes)
        return L"";
    std::vector<wchar_t> tmpString(numBytes);
    numBytes = GetEnvironmentVariableW(name.c_str(), &tmpString[0], static_cast<DWORD>(tmpString.size()));
    if(numBytes + 1 != static_cast<DWORD>(tmpString.size()))
        throw std::runtime_error("Invalid size returned");
    return std::wstring(tmpString.begin(), tmpString.end());
#else
    return cvUTF8ToWideString(getEnvVar(cvWideStringToUTF8(name)));
#endif // _WIN32
}

bfs::path System::getPathFromEnvVar(const std::string& name){
#ifdef _WIN32
    return getEnvVar(cvUTF8ToWideString(name));
#else
    return getEnvVar(name);
#endif // _WIN32
}

bool System::envVarExists(const std::string& name){
#ifdef _WIN32
    std::wstring wName = cvUTF8ToWideString(name);
    return GetEnvironmentVariableW(wName.c_str(), NULL, 0) > 0;
#else
    return getenv(name.c_str()) != NULL;
#endif // _WIN32
}

bfs::path System::getHomePath(){
    bfs::path homePath;

#ifdef _WIN32
    // "$User\Saved Games"
    std::string tmpPath;
    if(mySHGetKnownFolderPath(FOLDERID_SavedGames, tmpPath) == S_OK)
        homePath = tmpPath;
    else{
        // "$Documents\My Games"
        if(mySHGetKnownFolderPath(FOLDERID_Documents, tmpPath) == S_OK)
            homePath = bfs::path(tmpPath) / "My Games";
    }

    // No path found, use $AppData
    if(homePath.empty())
        homePath = getPathFromEnvVar("APPDATA");

#else // linux, apple
    homePath = getPathFromEnvVar("HOME");
#endif
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
#ifdef _WIN32
    DWORD nameLen = 0;
    // Query required size
    GetUserNameW(NULL, &nameLen);
    if(nameLen == 0)
        throw std::runtime_error("Could not query username length");
    std::vector<wchar_t> userName(nameLen);
    if(GetUserNameW(&userName[0], &nameLen) == 0)
        throw std::runtime_error("Could not get username");
    return cvWideStringToUTF8(std::wstring(userName.begin(), userName.end()));
#else
    return getEnvVar("USER");
#endif // _WIN32
}

bfs::path System::getExecutablePath(const char* argv0)
{
    return getExecutablePath(std::string(argv0 ? argv0 : ""));
}

bfs::path System::getExecutablePath(const std::string& argv0)
{
    return ::getExecutablePath(argv0);
}
