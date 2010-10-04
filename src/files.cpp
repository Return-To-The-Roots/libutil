// $Id: files.cpp 6582 2010-07-16 11:23:35Z FloSoft $
//
// Copyright (c) 2005 - 2010 Settlers Freaks (sf-team at siedler25.org)
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "files.h"

#ifdef _WIN32
#undef DATADIR
#include <shlobj.h>
#endif

#ifdef __CYGWIN__

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

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _WIN32

typedef HRESULT (WINAPI* LPSHGetKnownFolderPath)(REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR *ppszPath);

static LPSHGetKnownFolderPath gSHGetKnownFolderPath = NULL;
static HINSTANCE gShell32DLLInst = NULL;

static LPSTR UnicodeToAnsi(LPCWSTR s)
{
	if (s==NULL) 
		return NULL;

	int cw = lstrlenW(s);
	if (cw==0)
	{
		CHAR *psz = new CHAR[1];
		*psz = '\0';
		return psz;
	}
	
	int cc = WideCharToMultiByte(CP_ACP, 0, s, cw, NULL, 0, NULL, NULL);

	if (cc==0)
		return NULL;

	CHAR *psz = new CHAR[cc + 1];
	
	cc = WideCharToMultiByte(CP_ACP, 0, s, cw, psz, cc, NULL, NULL);
	if (cc == 0) 
	{
		delete[] psz;
		return NULL;
	}
	
	psz[cc] = '\0';
	return psz;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Wrapper um SHGetKnownFolderPath, unter Vista und Größer benutzt es das 
 *  originale SHGetKnownFolderPath, ansonsten SHGetFolderPath.
 *
 *  @param[in] rfid
 *  @param[in] path
 *
 *  @return liefert den Status zurück (S_OK bei OK)
 *
 *  @author FloSoft
 */
static HRESULT mySHGetKnownFolderPath(REFKNOWNFOLDERID rfid, std::string &path)
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
		if(SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PERSONAL|CSIDL_FLAG_CREATE, NULL, 0, ppszPath)))
			retval = S_OK;
	}

	if(ppszPath)
	{
		LPSTR ppszPathA = UnicodeToAnsi(ppszPath);
		path = ppszPathA;
		CoTaskMemFree(ppszPath);
		delete[] ppszPathA;
	}

	return retval;
}
#endif


///////////////////////////////////////////////////////////////////////////////
/**
 *  formt Pfade korrekt um.
 *
 *  @param[in] file
 *
 *  @return liefert den umgeformten Pfad zurück
 *
 *  @author FloSoft
 */
std::string GetFilePath(std::string file)
{
	std::string to = file;

	// ist der Pfad ein Home-Dir?
	if(file.at(0) == '~') 
	{
		std::stringstream s;
#ifdef _WIN32
		std::string path = "";

		// "$User\Saved Games"
		if(mySHGetKnownFolderPath(FOLDERID_SavedGames, path) != S_OK)
		{
			// "$Documents\My Games"
			if(mySHGetKnownFolderPath(FOLDERID_Documents, path) == S_OK)
			{
				path += "\\My Games";
			}
		}

		s << path;

		// Kein Pfad gefunden, $AppData verwenden
		if(s.str() == "")
			s << getenv("APPDATA");

// linux, apple
#else
		// $Home verwenden
		s << getenv("HOME");
#endif

		s << file.substr(1);

		to = s.str();
	}

	std::replace(to.begin(), to.end(), '\\', '/');
	return to;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  prüft ob eine Datei existiert (bzw ob sie lesbar ist)
 *
 *  @param[in] file
 *
 *  @return liefert ja oder nein zurück
 *
 *  @author FloSoft
 */
bool FileExists(std::string file)
{
	FILE *test = fopen(GetFilePath(file).c_str(), "rb");
	if(test)
	{
		fclose(test);
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  prüft ob eine Verzeichnis existiert (bzw ob es ein Verzeichnis ist)
 *
 *  @param[in] dir
 *
 *  @return liefert ja oder nein zurück
 *
 *  @author FloSoft
 */
bool IsDir(std::string dir)
{
	if(dir.at(dir.size()-1) == '/')
		dir.erase(dir.size()-1, 1);

#ifdef _WIN32
	std::string path = GetFilePath(dir).c_str();
	std::replace(path.begin(), path.end(), '/', '\\');

	HANDLE test;
	WIN32_FIND_DATAA wfd;

	test = FindFirstFileA(path.c_str(), &wfd);
	if(test != INVALID_HANDLE_VALUE)
	{
		FindClose(test);
		if( (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
			return true;
	}
#else
	DIR *test = opendir(GetFilePath(dir).c_str());
	if(test)
	{
		closedir(test);
		return true;
	}
#endif // !_WIN32

	return false;
}
