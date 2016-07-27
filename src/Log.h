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
#include <cstdarg>

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
        /// Write the last occured error description with "[text]: " at the front to stdOut and file
        void writeLastError(const char* text);

        /// Write formated text to stdOut and file
        FormatedLogEntry write(const std::string& format);
        /// Write formated text to stdOut and file
        FormatedLogEntry writeColored(const std::string& format, unsigned color);
        /// Write formated text to file only
        FormatedLogEntry writeToFile(const std::string& format);

        /// Write colored text to stdOut and file
        /// Deprecated! Uses the unsafe C format
        void writeColoredCFormat(const unsigned int color, const char* format, ...);
        /// Write formated text to stdOut and file
        /// Deprecated! Uses the unsafe C format
        void writeCFormat(const char* format, ...);

        /// Write formated text to file only
        /// Deprecated! Uses the unsafe C format
        void writeCFormatToFile(const char* format, ...);

        /// Set the console text color to one of the predefined colors
        void SetColor(unsigned color);
        /// Set the console text color back to original value
        void ResetColor();

        TextWriterInterface* getFileWriter() { return logFileWriter; }
    private:
        TextWriterInterface* logFileWriter;

        /// Write formated text to stdOut and file using a va_list
        void writeVArgs(const char* format, va_list list);
        /// Write formated text to file only using a va_list
        void writeToFileVArgs(const char* format, va_list list);

        void flushToStdOut(const char* txt);
        void flushToFile(const char* txt);
        // This 2 methods are used by the log entry holder
        friend class FormatedLogEntry;
};

#define LOG Log::inst()

#endif // LOG_H_INCLUDED
