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
#include "Log.h"
#include "FileWriter.h"
#include "MyTime.h"
#include "colors.h"
#include <boost/filesystem/path.hpp>
#include <boost/nowide/iostream.hpp>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#else
#include <cerrno>
#include <cstring>
#endif

Log::Log() : logFileWriter(NULL), logFilepath("logs") {}

Log::~Log()
{
    if(logFileWriter)
    {
        delete logFileWriter;
        logFileWriter = NULL;
    }
}

void Log::setLogFilepath(const std::string& filepath)
{
    if(logFileWriter)
        throw std::runtime_error("Cannot set log filepath after having already opened the log file");
    logFilepath = filepath;
}

void Log::open()
{
    if(!logFileWriter)
    {
        bfs::path filePath = bfs::path(logFilepath) / (libutil::Time::FormatTime("%Y-%m-%d_%H-%i-%s") + ".log");
        logFileWriter = new FileWriter(filePath.string());
    }
}

void Log::open(TextWriterInterface* fileWriter)
{
    if(logFileWriter)
    {
        delete fileWriter;
        throw std::runtime_error("Cannot reset already opened log file");
    }
    logFileWriter = fileWriter;
}

void Log::SetColor(unsigned color, bool stdoutOrStderr)
{
// On Linux, we insert escape-codes into the string. On Windows call system functions.
#ifndef _WIN32
    const char* colorModifier;

    // A switch statement doesn't work here because we compare against the array COLORS[] (although it's constant, it can't be dereferenced
    // at compile time)
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

    flush(colorModifier, stdoutOrStderr ? LogTarget::Stdout : LogTarget::Stderr);
#else
    // obtain handle
    HANDLE hStdout = GetStdHandle(stdoutOrStderr ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    if(GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
    {
        WORD colorAttr = 0;
        if(color == COLOR_BLUE || color == COLOR_MAGENTA || color == COLOR_CYAN || color == COLOR_WHITE)
            colorAttr |= FOREGROUND_BLUE;
        if(color == COLOR_YELLOW || color == COLOR_GREEN || color == COLOR_CYAN || color == COLOR_WHITE || color == COLOR_BROWN)
            colorAttr |= FOREGROUND_GREEN;
        if(color == COLOR_RED || color == COLOR_YELLOW || color == COLOR_MAGENTA || color == COLOR_WHITE || color == COLOR_BROWN)
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

void Log::ResetColor(bool stdoutOrStderr)
{
#ifndef _WIN32
    flush("\033[0m", stdoutOrStderr ? LogTarget::Stdout : LogTarget::Stderr);
#else
    HANDLE hStdout = GetStdHandle(stdoutOrStderr ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    if(GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
        SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
#endif
}

void Log::flush(const std::string& txt, LogTarget target)
{
    // Write to stdout or stderr
    if((target & LogTarget::Stdout) == LogTarget::Stdout)
        bnw::cout << txt << std::flush;
    else if((target & LogTarget::Stderr) == LogTarget::Stderr)
        bnw::cerr << txt << std::flush;
    // And possibly also to file
    if((target & LogTarget::File) == LogTarget::File)
    {
        open();
        logFileWriter->writeText(txt);
    }
}

/**
 *  Schreibt den zuletzt aufgetretetenen Systemfehler in lesbarer Form in
 *  stdout und Log, fügt "$text:" davor ein.
 */
void Log::writeLastError(const std::string& text)
{
    write("%s: %s\n") % text % getLastError();
}

std::string Log::getLastError() const
{
#ifdef _WIN32
    LPVOID lpMsgBuf;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(),
                   0, // Default language
                   (LPSTR)&lpMsgBuf, 0, NULL);
    std::string error = static_cast<const char*>(lpMsgBuf);
    // Free the buffer.
    LocalFree(lpMsgBuf);
    return error;
#else
    return strerror(errno);
#endif
}

FormatedLogEntry Log::write(const std::string& format, LogTarget target /* = LogTarget::FileAndStdout*/)
{
    return FormatedLogEntry(*this, target, format);
}

FormatedLogEntry Log::writeColored(const std::string& format, unsigned color, LogTarget target /* = LogTarget::FileAndStdout*/)
{
    return FormatedLogEntry(*this, target, format, color);
}

FormatedLogEntry Log::writeToFile(const std::string& format)
{
    return FormatedLogEntry(*this, LogTarget::File, format);
}
