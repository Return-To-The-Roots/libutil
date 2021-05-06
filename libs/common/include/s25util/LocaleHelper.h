// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <locale>

namespace LocaleHelper {
/// Init classic locale with UTF8 encoding for the global locale and boost::filesystem
/// Return true on success, false on error with an error message in stderr
bool init();
/// Returns the default locale as found in boost::filesystem
const std::locale& getBfsDefaultLocale();
} // namespace LocaleHelper
