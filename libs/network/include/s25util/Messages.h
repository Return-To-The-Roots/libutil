// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Message.h"
#include "MessageInterface.h"
#include "Protocol.h"

/*
 * das Klassenkommentar ist alles Client-Sicht, für Server-Sicht ist alles andersrum
 *
 * Konstruktor ohne Parameter ist allgemein nur zum Empfangen (immer noop!)
 * run-Methode ist Auswertung der Daten
 *
 * Konstruktor(en) mit Parametern (wenns auch nur der "reserved"-Parameter ist)
 * ist zum Verschicken der Nachrichten gedacht!
 */

///////////////////////////////////////////////////////////////////////////////
/// eingehende Null-Nachricht
class Message_Null : public Message
{
public:
    Message_Null() : Message(NMS_nullptr_MSG) {}
    bool run(MessageInterface* callback, unsigned id) override { return callback->OnNMSNull(id); }
};

///////////////////////////////////////////////////////////////////////////////
/// eingehende Dead-Nachricht
class Message_Dead : public Message
{
public:
    Message_Dead() : Message(NMS_DEAD_MSG) {}
    bool run(MessageInterface* callback, unsigned id) override { return callback->OnNMSDead(id); }
};
