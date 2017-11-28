// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
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
#include "StdStreamWriter.h"
#include <boost/filesystem/path.hpp>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#else
#include <cerrno>
#include <cstring>
#endif

Log::Log() : stdoutWriter(new StdStreamWriter(true)), stderrWriter(new StdStreamWriter(false)), logFilepath("logs") {}

Log::~Log() {}

void Log::setLogFilepath(const std::string& filepath)
{
    if(fileWriter)
        throw std::runtime_error("Cannot set log filepath after having already opened the log file");
    logFilepath = filepath;
}

void Log::open()
{
    if(!fileWriter)
    {
        bfs::path filePath = bfs::path(logFilepath) / (s25util::Time::FormatTime("%Y-%m-%d_%H-%i-%s") + ".log");
        fileWriter.reset(new FileWriter(filePath.string()));
    }
}

void Log::setWriter(TextWriterInterface* writer, LogTarget target)
{
    // Wrap in a shared so we can use it for more than 1 target
    boost::shared_ptr<TextWriterInterface> sharedWriter(writer);
    if((target & LogTarget::Stdout) == LogTarget::Stdout)
    {
        stdoutWriter = sharedWriter;
        if(!stdoutWriter)
            stdoutWriter.reset(new StdStreamWriter(true));
    }
    if((target & LogTarget::Stderr) == LogTarget::Stderr)
    {
        stderrWriter = sharedWriter;
        if(!stderrWriter)
            stderrWriter.reset(new StdStreamWriter(false));
    }
    if((target & LogTarget::File) == LogTarget::File)
        fileWriter = sharedWriter;
}

void Log::flush(const std::string& txt, LogTarget target, unsigned color)
{
    if((target & LogTarget::Stdout) == LogTarget::Stdout)
        stdoutWriter->writeText(txt, color);
    if((target & LogTarget::Stderr) == LogTarget::Stderr)
        stderrWriter->writeText(txt, color);
    if((target & LogTarget::File) == LogTarget::File)
    {
        open();
        fileWriter->writeText(txt, color);
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
