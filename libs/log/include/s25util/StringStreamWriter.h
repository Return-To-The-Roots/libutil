// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "TextWriterInterface.h"
#include <sstream>

class StringStreamWriter : public TextWriterInterface
{
    std::stringstream stream;

public:
    void writeText(const std::string& txt, unsigned color) override;

    std::string getText() const;
    std::stringstream& getStream() { return stream; }
};
