// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "libUtilDefines.h"
#include "Message.h"
#include "Messages.h"
#include <cstdio>

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author FloSoft
 */
Message* Message::create_base(unsigned short id)
{
    Message* msg = NULL;
    switch(id)
    {
        default: printf("ERROR: Message with id %u couldn't be created!\n", unsigned(id)); break;

        case NMS_NULL_MSG:  { msg = new Message_Null(); } break;
        case NMS_DEAD_MSG:  { msg = new Message_Dead(); } break;
    }

    return msg;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  dupliziert eine Nachricht.
 *
 *  @author FloSoft
 */
Message* Message::duplicate(void) const
{
    Message* msg = create(id_);
    Serializer ser;
    Serialize(ser);
    msg->Deserialize(ser);

    return msg;
}
