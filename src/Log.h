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

#include "Singleton.h"
#include "FormatedLogEntry.h"
#include <string>

class TextWriterInterface;

class Log : public Singleton<Log, SingletonPolicies::WithLongevity>
{
    public:
        static const unsigned Longevity = 100;

        Log();
        ~Log() override;

        /// Open the log file if it is not yet open
        void open();
        /// Use the given writer as the file writer
        void open(TextWriterInterface* fileWriter);
        /// Write the last occurred error description with "[text]: " at the front to stdOut and file
        void writeLastError(const char* text);

        /// Write formated text to target (default: File and Stdout)
        FormatedLogEntry write(const std::string& format, LogTarget target = LogTarget::FileAndStdout);
        FormatedLogEntry write(const char* format, LogTarget target = LogTarget::FileAndStdout);
        /// Write formated text to target (default: File and Stdout)
        FormatedLogEntry writeColored(const std::string& format, unsigned color, LogTarget target = LogTarget::FileAndStdout);
        FormatedLogEntry writeColored(const char* format, unsigned color, LogTarget target = LogTarget::FileAndStdout);
        /// Write formated text to file only
        FormatedLogEntry writeToFile(const std::string& format);
        FormatedLogEntry writeToFile(const char* format);

        /// Set the console text color to one of the predefined colors
        void SetColor(unsigned color, bool stdoutOrStderr);
        /// Set the console text color back to original value
        void ResetColor(bool stdoutOrStderr);

        TextWriterInterface* getFileWriter() { return logFileWriter; }
    private:
        TextWriterInterface* logFileWriter;

        void flush(const char* txt, LogTarget target);
        // This method is used by the log entry holder
        friend class FormatedLogEntry;
};

#define LOG Log::inst()

#endif // LOG_H_INCLUDED
