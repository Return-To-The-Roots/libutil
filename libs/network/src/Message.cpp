// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Message.h"
#include "Messages.h"
#include "Protocol.h"
#include "s25util/Serializer.h"
#include <cstdio>

Message* Message::create_base(unsigned short id)
{
    Message* msg = nullptr;
    switch(id)
    {
        default: printf("ERROR: Message with id %u couldn't be created!\n", unsigned(id)); break;

        case NMS_nullptr_MSG: msg = new Message_Null(); break;
        case NMS_DEAD_MSG: msg = new Message_Dead(); break;
    }

    return msg;
}

/**
 *  dupliziert eine Nachricht.
 */
Message* Message::clone() const
{
    Message* msg = create(msgId_);
    Serializer ser;
    Serialize(ser);
    msg->Deserialize(ser);

    return msg;
}
