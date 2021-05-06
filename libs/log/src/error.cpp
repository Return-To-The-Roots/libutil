// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "error.h"
#include "Log.h"

namespace s25util {

int fatal_error(const std::string& msg)
{
    LOG.write("\n\nFATAL ERROR: %1%\n") % msg;
    return EXIT_FAILURE;
}

int error(const std::string& msg)
{
    LOG.write("\n\nERROR: %1%\n") % msg;
    return EXIT_FAILURE;
}

int warning(const std::string& msg)
{
    LOG.write("\n\nWARNING: %1%\n") % msg;

    return EXIT_SUCCESS;
}

} // namespace s25util
