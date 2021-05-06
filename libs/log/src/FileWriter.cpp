// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "FileWriter.h"
#include <stdexcept>

FileWriter::FileWriter(const boost::filesystem::path& filePath)
{
    file.open(filePath);
    if(!file)
        throw std::runtime_error(std::string("Could not open ") + filePath.string() + " for writing");
}

void FileWriter::writeText(const std::string& txt, unsigned /*color*/)
{
    file << txt;
    file.flush();
}
