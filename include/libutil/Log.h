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

#pragma once

#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#include "FormatedLogEntry.h"
#include "Singleton.h"
#include <boost/shared_ptr.hpp>
#include <string>

class TextWriterInterface;

class Log : public Singleton<Log, SingletonPolicies::WithLongevity>
{
public:
    static const unsigned Longevity = 100;

    Log();
    ~Log() override;

    /// Sets the path where the log will be written to
    /// Must be done before the first write
    void setLogFilepath(const std::string& filepath);

    /// Open the log file if it is not yet open
    void open();
    /// Use the given writer for that target passing ownership
    /// Passing NULL resets it to the default
    void setWriter(TextWriterInterface* writer, LogTarget target);
    /// Write the last occurred error description with "[text]: " at the front to stdOut and file
    void writeLastError(const std::string& text);
    std::string getLastError() const;

    /// Write formated text to target (default: File and Stdout)
    FormatedLogEntry write(const std::string& format, LogTarget target = LogTarget::FileAndStdout);
    /// Write formated text to target (default: File and Stdout)
    FormatedLogEntry writeColored(const std::string& format, unsigned color, LogTarget target = LogTarget::FileAndStdout);
    /// Write formated text to file only
    FormatedLogEntry writeToFile(const std::string& format);

    boost::shared_ptr<TextWriterInterface> getStdoutWriter() { return stdoutWriter; }
    boost::shared_ptr<TextWriterInterface> getStderrWriter() { return stderrWriter; }
    boost::shared_ptr<TextWriterInterface> getFileWriter() { return fileWriter; }

private:
    boost::shared_ptr<TextWriterInterface> stdoutWriter, stderrWriter, fileWriter;
    /// Path where the log files are written to
    std::string logFilepath;

    void flush(const std::string& txt, LogTarget target, unsigned color = 0);
    // This method is used by the log entry holder
    friend class FormatedLogEntry;
};

#define LOG Log::inst()

#endif // LOG_H_INCLUDED
