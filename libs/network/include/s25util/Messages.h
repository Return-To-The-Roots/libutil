// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

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
