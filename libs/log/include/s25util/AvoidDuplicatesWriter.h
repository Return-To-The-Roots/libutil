// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "TextWriterInterface.h"
#include <memory>
#include <string>
#include <utility>

/// Adapter that avoids duplicate lines
class AvoidDuplicatesWriter : public TextWriterInterface
{
public:
    explicit AvoidDuplicatesWriter(std::shared_ptr<TextWriterInterface> writer) : origWriter(std::move(writer)) {}
    void writeText(const std::string& txt, unsigned color) override;
    void reset() { lastLine.clear(); }
    std::shared_ptr<TextWriterInterface> origWriter;

private:
    std::string lastLine;
};

inline void AvoidDuplicatesWriter::writeText(const std::string& txt, unsigned color)
{
    if(lastLine.find(txt) == 0)
        return;
    origWriter->writeText(txt, color);
    if(lastLine.find('\n') != std::string::npos)
        lastLine.clear();
    lastLine += txt;
}
