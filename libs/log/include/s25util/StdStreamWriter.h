// Copyright (c) 2016 - 2017 Settlers Freaks (sf-team at siedler25.org)
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

#ifndef StdStreamWriter_h__
#define StdStreamWriter_h__

#include "TextWriterInterface.h"
#include <iosfwd>

class StdStreamWriter : public TextWriterInterface
{
    const bool stdoutOrStderr_;
    std::ostream& os;

public:
    explicit StdStreamWriter(bool stdoutOrStderr) noexcept;

    void writeText(const std::string& txt, unsigned color) override;
    void setColor(unsigned color);
    void resetColor();
};

#endif // StdStreamWriter_h__