// Copyright (c) 2017 - 2017 Settlers Freaks (sf-team at siedler25.org)
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

#ifndef AvoidDuplicatesWriter_h__
#define AvoidDuplicatesWriter_h__

#include "TextWriterInterface.h"
#include <boost/shared_ptr.hpp>
#include <string>

/// Adapter that avoids duplicate lines
class AvoidDuplicatesWriter : public TextWriterInterface
{
public:
    AvoidDuplicatesWriter(boost::shared_ptr<TextWriterInterface> writer) : origWriter(writer) {}
    void writeText(const std::string& txt, unsigned color) override;
    void reset() { lastLine.clear(); }
    boost::shared_ptr<TextWriterInterface> origWriter;

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

#endif // AvoidDuplicatesWriter_h__
