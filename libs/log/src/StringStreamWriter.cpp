// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "StringStreamWriter.h"

std::string StringStreamWriter::getText() const
{
    return stream.str();
}

void StringStreamWriter::writeText(const std::string& txt, unsigned /*color*/)
{
    stream << txt;
}
