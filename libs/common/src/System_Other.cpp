// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "System.h"
#include <stdexcept>

boost::filesystem::path System::getHomePath()
{
    boost::filesystem::path homePath = getPathFromEnvVar("HOME");
    // Emergency fallback. Should never happen but prevents "~/foo"->"/foo"
    if(homePath.empty())
    {
        assert(false);
        homePath = ".";
    }
    return homePath;
}

std::string System::getUserName()
{
    std::string name = getEnvVar("USER");
    if(name.empty())
        throw std::runtime_error("Could not get username");
    return name;
}
