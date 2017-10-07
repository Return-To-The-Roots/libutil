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
#include "getExecutablePath.h"
#include "ucString.h"
#include <boost/system/api_config.hpp>
#include <cstdlib>
#ifdef BOOST_WINDOWS_API
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <stdexcept>
#endif // BOOST_WINDOWS_API

#if defined(__MINGW32__)

typedef GUID KNOWNFOLDERID;
#define REFKNOWNFOLDERID const KNOWNFOLDERID&

#define DEFINE_KNOWN_FOLDER(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    EXTERN_C const GUID DECLSPEC_SELECTANY name = {l, w1, w2, {b1, b2, b3, b4, b5, b6, b7, b8}}

#define KF_FLAG_CREATE 0x00008000

// {4C5C32FF-BB9D-43b0-B5B4-2D72E54EAAA4}
DEFINE_KNOWN_FOLDER(FOLDERID_SavedGames, 0x4c5c32ff, 0xbb9d, 0x43b0, 0xb5, 0xb4, 0x2d, 0x72, 0xe5, 0x4e, 0xaa, 0xa4);
// {FDD39AD0-238F-46AF-ADB4-6C85480369C7}
DEFINE_KNOWN_FOLDER(FOLDERID_Documents, 0xFDD39AD0, 0x238F, 0x46AF, 0xAD, 0xB4, 0x6C, 0x85, 0x48, 0x03, 0x69, 0xC7);

#elif defined BOOST_WINDOWS_API && (NTDDI_VERSION < NTDDI_VISTA)
#define KF_FLAG_CREATE 0x00008000
#endif // __CYGWIN__ || __MINGW32__

#ifdef BOOST_WINDOWS_API

typedef HRESULT(WINAPI* LPSHGetKnownFolderPath)(REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath);

static LPSHGetKnownFolderPath gSHGetKnownFolderPath = NULL;
static HINSTANCE gShell32DLLInst = NULL;

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

bool System::envVarExists(const std::string& name)
{
#ifdef BOOST_WINDOWS_API
    return envVarExists(cvUTF8ToWideString(name));
#else
    return getenv(name.c_str()) != NULL;
#endif // BOOST_WINDOWS_API
}

bool System::envVarExists(const std::wstring& name)
{
#ifdef BOOST_WINDOWS_API
    return GetEnvironmentVariableW(name.c_str(), NULL, 0) > 0;
#else
    return envVarExists(cvWideStringToUTF8(name));
#endif // BOOST_WINDOWS_API
}

std::string System::getEnvVar(const std::string& name)
{
#ifdef BOOST_WINDOWS_API
    return cvWideStringToUTF8(getEnvVar(cvUTF8ToWideString(name)));
#else
    const char* value = getenv(name.c_str());
    return value ? value : "";
#endif // BOOST_WINDOWS_API
}

std::wstring System::getEnvVar(const std::wstring& name)
{
#ifdef BOOST_WINDOWS_API
    // Get number of chars including terminating NULL
    DWORD numChars = GetEnvironmentVariableW(name.c_str(), NULL, 0);
    if(!numChars)
        return L"";
    std::vector<wchar_t> tmpString(numChars);
    numChars = GetEnvironmentVariableW(name.c_str(), &tmpString[0], static_cast<DWORD>(tmpString.size()));
    // This does NOT include the terminating NULL
    if(numChars + 1 != static_cast<DWORD>(tmpString.size()))
        throw std::runtime_error("Invalid size returned");
    // Convert to wstring but do not include terminating 0
    return std::wstring(tmpString.begin(), tmpString.begin() + numChars);
#else
    return cvUTF8ToWideString(getEnvVar(cvWideStringToUTF8(name)));
#endif // BOOST_WINDOWS_API
}

bfs::path System::getPathFromEnvVar(const std::string& name)
{
#ifdef BOOST_WINDOWS_API
    return getEnvVar(cvUTF8ToWideString(name));
#else
    return getEnvVar(name);
#endif // BOOST_WINDOWS_API
}

bool System::setEnvVar(const std::string& name, const std::string& value)
{
#ifdef BOOST_WINDOWS_API
    return setEnvVar(cvUTF8ToWideString(name), cvUTF8ToWideString(value));
#else
    return setenv(name.c_str(), value.c_str(), 1) == 0;
#endif // BOOST_WINDOWS_API
}

bool System::setEnvVar(const std::wstring& name, const std::wstring& value)
{
#ifdef BOOST_WINDOWS_API
    return SetEnvironmentVariableW(name.c_str(), value.c_str()) == TRUE;
#else
    return setEnvVar(cvWideStringToUTF8(name), cvWideStringToUTF8(value));
#endif // BOOST_WINDOWS_API
}

bool System::removeEnvVar(const std::string& name)
{
#ifdef BOOST_WINDOWS_API
    return removeEnvVar(cvUTF8ToWideString(name));
#else
    unsetenv(name.c_str());
    return true;
#endif // BOOST_WINDOWS_API
}

bool System::removeEnvVar(const std::wstring& name)
{
#ifdef BOOST_WINDOWS_API
    return SetEnvironmentVariableW(name.c_str(), NULL) == TRUE;
#else
    return removeEnvVar(cvWideStringToUTF8(name));
#endif // BOOST_WINDOWS_API
}

bfs::path System::getHomePath()
{
    bfs::path homePath;

#ifdef BOOST_WINDOWS_API
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
#ifdef BOOST_WINDOWS_API
    DWORD nameLen = 0;
    // Query required size
    GetUserNameW(NULL, &nameLen);
    if(nameLen == 0)
        throw std::runtime_error("Could not query username length");
    std::vector<wchar_t> userName(nameLen);
    if(GetUserNameW(&userName[0], &nameLen) == 0)
        throw std::runtime_error("Could not get username");

    userName.resize(nameLen - 1); // nameLen already contains terminating 0

    return cvWideStringToUTF8(std::wstring(userName.begin(), userName.end()));
#else
    return getEnvVar("USER");
#endif // BOOST_WINDOWS_API
}

bfs::path System::getExecutablePath(const char* argv0)
{
    return getExecutablePath(std::string(argv0 ? argv0 : ""));
}

bfs::path System::getExecutablePath(const std::string& argv0)
{
    return ::getExecutablePath(argv0);
}
