// $Id: files.h 7521 2011-09-08 20:45:55Z FloSoft $
//
// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
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
#ifndef FILES_H_INCLUDED
#define FILES_H_INCLUDED

#pragma once

#ifndef PREFIX
#	ifdef _WIN32
#		define PREFIX "."
#	else
#		define PREFIX "."
#	endif
#endif // !PREFIX

#ifndef BINDIR
#	ifdef _WIN32
#		define BINDIR PREFIX
#	else
#		define BINDIR PREFIX"/bin"
#	endif
#endif // !BINDIR

#ifndef DATADIR
#	ifdef _WIN32
#		define DATADIR PREFIX
#	else
#		define DATADIR PREFIX"/share/s25rttr"
#	endif
#endif // !DATADIR

#ifndef GAMEDIR
#	ifdef _WIN32
#		define GAMEDIR PREFIX
#	else
#		define GAMEDIR DATADIR"/S2"
#	endif
#endif // !GAMEDIR

#ifndef DRIVERDIR
#	ifdef _WIN32
#		define DRIVERDIR DATADIR"/driver"
#	else
#		define DRIVERDIR DATADIR"/driver"
#	endif
#endif // !DRIVERDIR

#ifndef RTTRDIR
#	ifdef _WIN32
#		define RTTRDIR DATADIR"/RTTR"
#	else
#		define RTTRDIR DATADIR"/RTTR"
#	endif
#endif // !DRIVERDIR

#ifndef SETTINGSDIR
#	if defined(_WIN32)
#		define SETTINGSDIR "~/Return To The Roots"
#	elif defined(__APPLE__)
#		define SETTINGSDIR "~/Library/Application Support/Return To The Roots"
#	else
#		define SETTINGSDIR "~/.s25rttr"
#	endif
#endif // !SETTINGSDIR

///////////////////////////////////////////////////////////////////////////////
// Konstanten
const char * const FILE_PATHS[] =
{
	/*  0 */SETTINGSDIR "/CONFIG.INI",                // die Einstellungsdatei
	/*  1 */RTTRDIR "",                               // unbenutzt
	/*  2 */GAMEDIR "",                               // unbenutzt
	/*  3 */GAMEDIR "",                               // unbenutzt
	/*  4 */RTTRDIR "",                               // unbenutzt
	/*  5 */GAMEDIR "/GFX/PALETTE/PAL5.BBM",          // die ganzen Paletten
	/*  6 */GAMEDIR "/GFX/PALETTE/PAL6.BBM",
	/*  7 */GAMEDIR "/GFX/PALETTE/PAL7.BBM",
	/*  8 */GAMEDIR "/GFX/PALETTE/PALETTI0.BBM",
	/*  9 */GAMEDIR "/GFX/PALETTE/PALETTI1.BBM",
	/* 10 */GAMEDIR "/GFX/PALETTE/PALETTI8.BBM",
	/* 11 */GAMEDIR "/DATA/RESOURCE.DAT",
	/* 12 */GAMEDIR "/DATA/IO/IO.DAT",
	/* 13 */GAMEDIR "",                               // unbenutzt
	/* 14 */GAMEDIR "",                               // unbenutzt
	/* 15 */RTTRDIR "/languages",                     // die ganzen Sprachdateien
	/* 16 */RTTRDIR "/RTTR.LST",
	/* 17 */RTTRDIR "/COLORS.ACT",                    // Spezialpalette wegen Schriften usw
	/* 18 */RTTRDIR "",                               // unbenutzt
	/* 19 */GAMEDIR "",                               // unbenutzt
	/* 20 */GAMEDIR "/GFX/TEXTURES/TEX5.LBM",         // Grünland
	/* 21 */GAMEDIR "/GFX/TEXTURES/TEX6.LBM",         // Ödland
	/* 22 */GAMEDIR "/GFX/TEXTURES/TEX7.LBM",         // Winterwelt
	/* 23 */GAMEDIR "/DATA/MAP_0_Z.LST",              // Grünland
	/* 24 */GAMEDIR "/DATA/MAP_1_Z.LST",              // Ödland
	/* 25 */GAMEDIR "/DATA/MAP_2_Z.LST",              // Winterwelt
	/* 26 */GAMEDIR "/DATA/CBOB/ROM_BOBS.LST",
	/* 27 */GAMEDIR "/DATA/MBOB/AFR_Z.LST",           // Afrikaner
	/* 28 */GAMEDIR "/DATA/MBOB/JAP_Z.LST",           // Japaner
	/* 29 */GAMEDIR "/DATA/MBOB/ROM_Z.LST",           // Römer
	/* 30 */GAMEDIR "/DATA/MBOB/VIK_Z.LST",           // Wikinger
	/* 31 */GAMEDIR "/DATA/MBOB/WAFR_Z.LST",          // Afrikaner Winter
	/* 32 */GAMEDIR "/DATA/MBOB/WJAP_Z.LST",          // Japaner Winter
	/* 33 */GAMEDIR "/DATA/MBOB/WROM_Z.LST",          // Römer Winter
	/* 34 */GAMEDIR "/DATA/MBOB/WVIK_Z.LST",          // Wikinger Winter
	/* 35 */GAMEDIR "/DATA/MBOB/AFR_ICON.LST",
	/* 36 */GAMEDIR "/DATA/MBOB/JAP_ICON.LST",
	/* 37 */GAMEDIR "/DATA/MBOB/ROM_ICON.LST",
	/* 38 */GAMEDIR "/DATA/MBOB/VIK_ICON.LST",
	/* 39 */GAMEDIR "/DATA/MAPS3/",
	/* 40 */GAMEDIR "/DATA/MAPS4/",
	/* 41 */GAMEDIR "/WORLDS/",
	/* 42 */GAMEDIR "/DATA/MAPS2/",
	/* 43 */GAMEDIR "/DATA/MAPS/",
	/* 44 */GAMEDIR "/DATA/BOBS/CARRIER.BOB",
	/* 45 */GAMEDIR "/DATA/BOBS/JOBS.BOB",
	/* 46 */DRIVERDIR "/",                            // Treiberordner
	/* 47 */SETTINGSDIR "/LOGS/",                     // Log-Ordner
	/* 48 */SETTINGSDIR "/MAPS/",                     // die heruntergeladenen Karten
	/* 49 */GAMEDIR "/DATA/SOUNDDAT/SOUND.LST",       // die originale sound.lst
	/* 50 */RTTRDIR "/MUSIC/SNG/SNG_*.OGG",           // die musik
	/* 51 */SETTINGSDIR "/REPLAYS/",                  // Replayordner
	/* 52 */RTTRDIR "/MAPS/NEW/",                     // unsere eigenen neuen Karten
	/* 53 */GAMEDIR "/DATA/SOUNDDAT/SNG/SNG_*.DAT",   // die originale sound.lst
	/* 54 */RTTRDIR "",                               // unbenutzt
	/* 55 */SETTINGSDIR "/LSTS/SOUND.LST",            // Die konvertierte sound.lst
	/* 56 */RTTRDIR "/sound.scs",                     // Das konvertier-script
	/* 57 */RTTRDIR "",                               // Basispfad fürn Soundconverter
	/* 58 */GAMEDIR "/DATA/MIS0BOBS.LST",
	/* 59 */GAMEDIR "/DATA/MIS1BOBS.LST",
	/* 60 */GAMEDIR "/DATA/MIS2BOBS.LST",
	/* 61 */GAMEDIR "/DATA/MIS3BOBS.LST",
	/* 62 */GAMEDIR "/DATA/MIS4BOBS.LST",
	/* 63 */GAMEDIR "/DATA/MIS5BOBS.LST",
	/* 64 */GAMEDIR "/GFX/PICS/SETUP666.LBM",          // die Karten-Lade-Screens
	/* 65 */GAMEDIR "/GFX/PICS/SETUP667.LBM",
	/* 66 */GAMEDIR "/GFX/PICS/SETUP801.LBM",
	/* 67 */GAMEDIR "/GFX/PICS/SETUP802.LBM",
	/* 68 */GAMEDIR "/GFX/PICS/SETUP803.LBM",
	/* 69 */GAMEDIR "/GFX/PICS/SETUP804.LBM",
	/* 70 */GAMEDIR "/GFX/PICS/SETUP805.LBM",
	/* 71 */GAMEDIR "/GFX/PICS/SETUP806.LBM",
	/* 72 */GAMEDIR "/GFX/PICS/SETUP810.LBM",
	/* 73 */GAMEDIR "/GFX/PICS/SETUP811.LBM",
	/* 74 */GAMEDIR "/GFX/PICS/SETUP895.LBM",
	/* 75 */GAMEDIR "/GFX/PICS/SETUP896.LBM",
	/* 76 */GAMEDIR "/GFX/PICS/MISSION/AFRICA.LBM",
	/* 77 */GAMEDIR "/GFX/PICS/MISSION/AUSTRA.LBM",
	/* 78 */GAMEDIR "/GFX/PICS/MISSION/EUROPE.LBM",
	/* 79 */GAMEDIR "/GFX/PICS/MISSION/GREEN.LBM",
	/* 80 */GAMEDIR "/GFX/PICS/MISSION/JAPAN.LBM",
	/* 81 */GAMEDIR "/GFX/PICS/MISSION/NAMERICA.LBM",
	/* 82 */GAMEDIR "/GFX/PICS/MISSION/NASIA.LBM",
	/* 83 */GAMEDIR "/GFX/PICS/MISSION/SAMERICA.LBM",
	/* 84 */GAMEDIR "/GFX/PICS/MISSION/SASIA.LBM",
	/* 85 */SETTINGSDIR "/SAVE/",                     // Der Speicherordner
	/* 86 */GAMEDIR "/DATA/BOBS/BOAT.LST",
	/* 87 */RTTRDIR "",                               // unbenutzt
	/* 88 */RTTRDIR "/texte/",
	/* 89 */RTTRDIR "",                               // unbenutzt
	/* 90 */RTTRDIR "/MUSIC/",
	/* 91 */RTTRDIR "/MAPS/OTHER/",                   // Andere Maps
	/* 92 */GAMEDIR "/DATA/BOOT_Z.LST",
	/* 93 */RTTRDIR "/MAPS/SEA/",                     // Seefahrtkarten
	/* 94 */SETTINGSDIR,                              // Einstellungsordner
	/* 95 */RTTRDIR "/LSTS/",                         // systemweite lstfiles (immer bei start geladen)
	/* 96 */RTTRDIR "/LSTS/GAME/",                    // systemweite lstfiles (immer bei spielstart geladen)
	/* 97 */RTTRDIR "",                               // unbenutzt
	/* 98 */SETTINGSDIR "/LSTS/",                     // persönliche lstfiles (immer bei start geladen)
	/* 99 */SETTINGSDIR "/LSTS/GAME/",                // persönliche lstfiles (immer bei spielstart geladen)
	/*100 */SETTINGSDIR "",                           // unbenutzt
	/*101 */RTTRDIR "",                               // unbenutzt
	/*102 */GAMEDIR "/GFX/PICS/SETUP013.LBM",         // Optionen
	/*103 */GAMEDIR "/GFX/PICS/SETUP015.LBM",         // Freies Spiel
	/*104 */RTTRDIR "/LSTS/splash.bmp"                // Splash
};

const unsigned int FILE_SPLASH_ID = 104;

const unsigned int FILE_LOAD_IDS_COUNT = 21;
const std::string FILE_LOAD_IDS[] = {
	"setup666", "setup667", 
	"setup801", "setup802", "setup803", "setup804", "setup805", "setup806",
	"setup810", "setup811",
	"setup895", "setup896",
	"africa", "austra", "europe", "green", "japan", "namerica", "nasia", "samerica", "sasia"
};

const std::string CONFIG_NAME = "config";
const unsigned int GFXSET_COUNT = 3;
const unsigned int NATION_COUNT = 4;

const std::string NATION_ICON_IDS[NATION_COUNT] = {
	"afr_icon", "jap_icon", "rom_icon", "vik_icon"
};

const std::string NATION_GFXSET_Z[GFXSET_COUNT][NATION_COUNT] = {
	{"afr_z", "jap_z", "rom_z", "vik_z"},
	{"afr_z", "jap_z", "rom_z", "vik_z"},
	{"wafr_z", "wjap_z", "wrom_z", "wvik_z"}
};

const std::string MAP_GFXSET_Z[GFXSET_COUNT] = {
	"map_0_z", "map_1_z", "map_2_z"
};

const std::string TEX_GFXSET[GFXSET_COUNT] = {
	"tex5", "tex6", "tex7"
};

std::string GetFilePath(std::string file);
bool FileExists(std::string file);
bool IsDir(std::string dir);

#endif // FILES_H_INCLUDED
