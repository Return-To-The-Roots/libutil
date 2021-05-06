// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>

/// Interface for writing text to a target
class TextWriterInterface
{
public:
    virtual ~TextWriterInterface() = default;
    virtual void writeText(const std::string& txt, unsigned color) = 0;
};
