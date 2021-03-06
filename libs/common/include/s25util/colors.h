// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "warningSuppression.h"
#include <array>

/// Farbkonstanten
const unsigned COLOR_BLUE = 0xFF0073FF;
const unsigned COLOR_RED = 0xFFFF0000;
const unsigned COLOR_YELLOW = 0xFFFFFF00;
const unsigned COLOR_GREEN = 0xFF00FF00;
const unsigned COLOR_MAGENTA = 0xFFFF00FF;
const unsigned COLOR_CYAN = 0xFF00FFFF;
const unsigned COLOR_BLACK = 0xFF000000;
const unsigned COLOR_WHITE = 0xFFFFFFFF;
const unsigned COLOR_ORANGE = 0xFFFF8000;
const unsigned COLOR_BROWN = 0xFF6C4337;
const unsigned COLOR_PURPLE = 0xFF7F00FF;
/// const unsigned COLOR_GHOST = 0x05FFFFFF; Demos Ghost-Farbe
const unsigned COLOR_SHADOW = 0x40000000;
const unsigned COLOR_GREY = 0xFFCCCCCC;
const unsigned COLOR_WINDOWBROWN = 0xFF886034;

/// Colors used when showing percentages
const unsigned COLOR_0_PERCENT = 0xFFD70000;
const unsigned COLOR_20_PERCENT = 0xFFDB7428;
const unsigned COLOR_30_PERCENT = 0xFFFFBF33;
const unsigned COLOR_60_PERCENT = 0xFF71B63C;

/// Spielerfarben
const std::array<unsigned, 11> SUPPRESS_UNUSED PLAYER_COLORS = {{
  0xFF0079DB,   // blue
  0xFFCBCB00,   // yellow
  0xFFD70000,   // red
  0xFF9A75BE,   // magenta
  0xFF4D4D4D,   // black
  0xFF41C371,   // green
  COLOR_ORANGE, // orange
  COLOR_CYAN,   // xyan,
  COLOR_WHITE,  // white
  COLOR_BROWN,  // brown
  COLOR_PURPLE, // purple
}};

/// Liefert die transparente Farbkomponente zurück.
inline unsigned GetAlpha(const unsigned color)
{
    return ((color & 0xFF000000) >> 24);
}
/// Liefert die rote Farbkomponente zurück.
inline unsigned GetRed(const unsigned color)
{
    return ((color & 0x00FF0000) >> 16);
}
/// Liefert die grüne Farbkomponente zurück.
inline unsigned GetGreen(const unsigned color)
{
    return ((color & 0x0000FF00) >> 8);
}
/// Liefert die blaue Farbkomponente zurück.
inline unsigned GetBlue(const unsigned color)
{
    return ((color & 0x000000FF));
}

/// Setzt die transparente Farbkomponente.
inline unsigned SetAlpha(const unsigned color, const unsigned alpha)
{
    return ((color & 0x00FFFFFF) | (alpha << 24));
}
/// Setzt die rote Farbkomponente.
inline unsigned SetRed(const unsigned color, const unsigned red)
{
    return ((color & 0xFF00FFFF) | (red << 16));
}
/// Setzt die grüne Farbkomponente.
inline unsigned SetGreen(const unsigned color, const unsigned green)
{
    return ((color & 0xFFFF00FF) | (green << 8));
}
/// Setzt die blaue Farbkomponente.
inline unsigned SetBlue(const unsigned color, const unsigned blue)
{
    return ((color & 0xFFFFFF00) | (blue));
}

/// Erzeugt eine fertige Farbe aus einzelnen Farbkomponenten
inline unsigned MakeColor(const unsigned alpha, const unsigned red, const unsigned green, const unsigned blue)
{
    return ((alpha << 24) | (red << 16) | (green << 8) | blue);
}
