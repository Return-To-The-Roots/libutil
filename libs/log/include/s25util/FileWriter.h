// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "TextWriterInterface.h"
#include <boost/filesystem/path.hpp>
#include <boost/nowide/fstream.hpp>

class FileWriter : public TextWriterInterface
{
    boost::nowide::ofstream file;

public:
    explicit FileWriter(const boost::filesystem::path& filePath);
    void writeText(const std::string& txt, unsigned color) override;
};
