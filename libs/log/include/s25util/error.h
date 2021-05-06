// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>

namespace s25util {

/// Write the message with fatal-error tag and return EXIT_FAILURE
int fatal_error(const std::string& msg);

/// Write the message with error tag and return EXIT_FAILURE
int error(const std::string& msg);

/// Write the message with warning tag and return EXIT_SUCCESS
int warning(const std::string& msg);

} // namespace s25util
