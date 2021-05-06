// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

class MessageInterface
{
protected:
    virtual ~MessageInterface() = default;

public:
    /// Null-Nachricht.
    virtual bool OnNMSNull(unsigned) { return false; }

    /// Dead-Nachricht.
    virtual bool OnNMSDead(unsigned) { return false; }
};
