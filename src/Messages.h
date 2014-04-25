// $Id: Messages.h 9359 2014-04-25 15:37:22Z FloSoft $
//
// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
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
#ifndef MESSAGES_H_INCLUDED
#define MESSAGES_H_INCLUDED

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
        Message_Null(void) : Message(NMS_NULL_MSG) { }
        Message_Null(bool reserved) : Message(NMS_NULL_MSG)
        {
            LOG.write(">>> NMS_NULL_MSG\n");
        }
        void run(MessageInterface* callback, unsigned int id)
        {
            LOG.write("<<< NMS_NULL_MSG\n");
            callback->OnNMSNull(id);
        }
};


///////////////////////////////////////////////////////////////////////////////
/// eingehende Dead-Nachricht
class Message_Dead : public Message
{
    public:
        Message_Dead(void) : Message(NMS_DEAD_MSG) { }
        Message_Dead(bool reserved) : Message(NMS_DEAD_MSG)
        {
            LOG.write(">>> NMS_DEAD_MSG\n");
        }
        void run(MessageInterface* callback, unsigned int id)
        {
            LOG.write("<<< NMS_DEAD_MSG\n");
            callback->OnNMSDead(id);
        }
};

#endif // !MESSAGES_H_INCLUDED
