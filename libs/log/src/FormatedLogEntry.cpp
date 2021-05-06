// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "FormatedLogEntry.h"
#include "Log.h"

// NOLINTNEXTLINE(bugprone-exception-escape)
FormatedLogEntry::~FormatedLogEntry() noexcept(false)
{
    std::string msg = fmt.str();
    // Empty messages also occur when this has been moved from
    if(!msg.empty())
        log_.flush(msg, target_, color_);
}
