// $Id: colors.h 8827 2013-08-03 15:42:22Z marcus $
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
#ifndef COLORS_H_INCLUDED
#define COLORS_H_INCLUDED

#pragma once

/// Texturfarben
enum TextureColor
{
	TC_GREY = 0,
	TC_RED1,
	TC_GREEN1,
	TC_GREEN2,
	TC_RED2,
	TC_BRICKS,
	TC_INVISIBLE
};


/// Farbkonstanten
const unsigned COLOR_BLUE = 0xFF0073FF;
const unsigned COLOR_RED = 0xFFFF0000;
const unsigned COLOR_YELLOW	= 0xFFFFFF00;
const unsigned COLOR_GREEN	= 0xFF00FF00;
const unsigned COLOR_MAGENTA = 0xFFFF00FF;
const unsigned COLOR_CYAN = 0xFF00FFFF;
const unsigned COLOR_BLACK = 0xFF444444;
const unsigned COLOR_WHITE = 0xFFFFFFFF;
const unsigned COLOR_ORANGE = 0xFFFF8000;
const unsigned COLOR_BROWN = 0xFF6C4337;
const unsigned COLOR_PURPLE = 0xFF7F00FF;
/// const unsigned COLOR_GHOST = 0x05FFFFFF; Demos Ghost-Farbe
const unsigned COLOR_SHADOW = 0x40000000;
const unsigned COLOR_GREY = 0xFFCCCCCC;
const unsigned COLOR_WINDOWBROWN = 0xFF886034;

/// Spielerfarben
const unsigned int PLAYER_COLORS_COUNT = 11;

const unsigned int COLORS[PLAYER_COLORS_COUNT] =
{
	0xFF0079DB,	// COLOR_BLUE, // Blau
	0xFFD70000,	// COLOR_RED, // Rot
	0xFFCBCB00,	//COLOR_YELLOW, // Gelb
	0xFF41C371,	// COLOR_GREEN, // Gruen
	0xFF9A75BE,	// COLOR_MAGENTA, // Magenta
	COLOR_CYAN, // Cyan,
	0xFF4D4D4D,	// COLOR_BLACK, // Schwarz
	COLOR_WHITE, // Weiss
	COLOR_ORANGE, // Orange
	COLOR_BROWN, // Braun
	COLOR_PURPLE, // Lila
///	COLOR_GHOST // Durchsichtig
};

/// Liefert die transparente Farbkomponente zurück.
inline unsigned GetAlpha(const unsigned int color) { return (( color & 0xFF000000) >> 24); }
/// Liefert die rote Farbkomponente zurück.
inline unsigned GetRed(  const unsigned int color) { return (( color & 0x00FF0000) >> 16); }
/// Liefert die grüne Farbkomponente zurück.
inline unsigned GetGreen(const unsigned int color) { return (( color & 0x0000FF00) >> 8 ); }
/// Liefert die blaue Farbkomponente zurück.
inline unsigned GetBlue( const unsigned int color) { return (( color & 0x000000FF)      ); }

/// Setzt die transparente Farbkomponente.
inline unsigned SetAlpha(const unsigned int color, const unsigned int alpha) { return (( color & 0x00FFFFFF) | ( alpha << 24) ); }
/// Setzt die rote Farbkomponente.
inline unsigned SetRed(  const unsigned int color, const unsigned int red  ) { return (( color & 0xFF00FFFF) | ( red   << 16) ); }
/// Setzt die grüne Farbkomponente.
inline unsigned SetGreen(const unsigned int color, const unsigned int green) { return (( color & 0xFFFF00FF) | ( green << 8 ) ); }
/// Setzt die blaue Farbkomponente.
inline unsigned SetBlue( const unsigned int color, const unsigned int blue ) { return (( color & 0xFFFFFF00) | ( blue       ) ); }

/// Erzeugt eine fertige Farbe aus einzelnen Farbkomponenten
inline unsigned MakeColor(const unsigned int alpha, const unsigned int red, const unsigned int green, const unsigned int blue) 
{ 
	return ((alpha << 24) | (red << 16) | (green << 8) |  blue);
}

#endif // COLORS_H_INCLUDED
