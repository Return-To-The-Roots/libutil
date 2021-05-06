// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "FormatedLogEntry.h"
#include "s25util/Singleton.h"
#include <boost/config.hpp>
#include <boost/filesystem/path.hpp>
#include <memory>
#include <string>

class TextWriterInterface;

class Log : public Singleton<Log, SingletonPolicies::WithLongevity>
{
public:
    static const unsigned Longevity = 100;

    Log();
    ~Log();

    /// Sets the path where the log will be written to
    /// Must be done before the first write
    void setLogFilepath(boost::filesystem::path filepath);

    /// Open the log file if it is not yet open
    void open();
    /// Use the given writer for that target passing ownership
    /// Passing nullptr resets it to the default
    void setWriter(TextWriterInterface* writer, LogTarget target);
    /// Write the last occurred error description with "[text]: " at the front to stdOut and file
    void writeLastError(const std::string& text);
    static std::string getLastError();

    /// Write formated text to target (default: File and Stdout)
    FormatedLogEntry write(const char* format, LogTarget target = LogTarget::FileAndStdout);
    /// Write formated text to target (default: File and Stdout)
    FormatedLogEntry writeColored(const char* format, unsigned color, LogTarget target = LogTarget::FileAndStdout);
    /// Write formated text to file only
    FormatedLogEntry writeToFile(const char* format);

    std::shared_ptr<TextWriterInterface> getStdoutWriter() { return stdoutWriter; }
    std::shared_ptr<TextWriterInterface> getStderrWriter() { return stderrWriter; }
    std::shared_ptr<TextWriterInterface> getFileWriter() { return fileWriter; }

private:
    std::shared_ptr<TextWriterInterface> stdoutWriter, stderrWriter, fileWriter;
    /// Path where the log files are written to
    boost::filesystem::path logFilepath;

    void flush(const std::string& txt, LogTarget target, unsigned color = 0);
    // This method is used by the log entry holder
    friend class FormatedLogEntry;
};

#define LOG Log::inst()
