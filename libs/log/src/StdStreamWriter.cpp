// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "StdStreamWriter.h"
#include "s25util/colors.h"
#include <boost/nowide/iostream.hpp>
#ifdef _WIN32
#    include <windows.h>
#endif

namespace bnw = boost::nowide;

StdStreamWriter::StdStreamWriter(bool stdoutOrStderr) noexcept
    : stdoutOrStderr_(stdoutOrStderr), os(stdoutOrStderr_ ? bnw::cout : bnw::cerr)
{}

void StdStreamWriter::writeText(const std::string& txt, unsigned color)
{
    if(color)
        setColor(color);
    os << txt << std::flush;
    if(color)
        resetColor();
}

void StdStreamWriter::setColor(unsigned color)
{
// On Linux, we insert escape-codes into the string. On Windows call system functions.
#ifndef _WIN32
    const char* colorModifier;

    // A switch statement doesn't work here because we compare against the array COLORS[] (although it's constant, it
    // can't be dereferenced at compile time)
    if(color == COLOR_BLUE)
        colorModifier = "\033[40m\033[1;34m";
    else if(color == COLOR_RED)
        colorModifier = "\033[40m\033[1;31m";
    else if(color == COLOR_YELLOW)
        colorModifier = "\033[40m\033[1;33m";
    else if(color == COLOR_GREEN)
        colorModifier = "\033[40m\033[1;32m";
    else if(color == COLOR_MAGENTA)
        colorModifier = "\033[40m\033[1;35m";
    else if(color == COLOR_CYAN)
        colorModifier = "\033[40m\033[1;36m";
    else if(color == COLOR_BLACK)
        colorModifier = "\033[47m\033[1;30m";
    else if(color == COLOR_WHITE)
        colorModifier = "\033[40m\033[1;37m";
    else if(color == COLOR_ORANGE)
        colorModifier = "\033[43m\033[1;30m";
    else if(color == COLOR_BROWN)
        colorModifier = "\033[40m\033[33m";
    else
        colorModifier = "\033[0m";

    os << colorModifier;
#else
    // obtain handle
    HANDLE hStdout = GetStdHandle(stdoutOrStderr_ ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    if(GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
    {
        WORD colorAttr = 0;
        if(color == COLOR_BLUE || color == COLOR_MAGENTA || color == COLOR_CYAN || color == COLOR_WHITE)
            colorAttr |= FOREGROUND_BLUE;
        if(color == COLOR_YELLOW || color == COLOR_GREEN || color == COLOR_CYAN || color == COLOR_WHITE
           || color == COLOR_BROWN)
            colorAttr |= FOREGROUND_GREEN;
        if(color == COLOR_RED || color == COLOR_YELLOW || color == COLOR_MAGENTA || color == COLOR_WHITE
           || color == COLOR_BROWN)
            colorAttr |= FOREGROUND_RED;
        if(color == COLOR_BLACK)
            colorAttr |= BACKGROUND_BLUE;
        if(color == COLOR_BLACK || color == COLOR_ORANGE)
            colorAttr |= BACKGROUND_GREEN | BACKGROUND_RED;

        // if color matches any but brown
        if(colorAttr != 0 && color != COLOR_BROWN)
            colorAttr |= FOREGROUND_INTENSITY;

        // if color didn't match any, make default gray
        if(colorAttr == 0)
            colorAttr = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;

        SetConsoleTextAttribute(hStdout, colorAttr);
    }
#endif
}

void StdStreamWriter::resetColor()
{
#ifndef _WIN32
    os << "\033[0m";
#else
    HANDLE hStdout = GetStdHandle(stdoutOrStderr_ ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    if(GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
        SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
#endif
}
