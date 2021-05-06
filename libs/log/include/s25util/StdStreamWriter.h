// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

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
