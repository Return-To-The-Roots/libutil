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
#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#pragma once

#include <stdint.h>

class MessageInterface;
class Serializer;

class Message
{
    uint16_t msgId_;

public:
    Message(uint16_t id) : msgId_(id) {}
    virtual ~Message() {}

    virtual void Serialize(Serializer& /*ser*/) const {}
    virtual void Deserialize(Serializer& /*ser*/) {}

    unsigned short getId() const { return msgId_; }
    static Message* create_base(unsigned short id);

    virtual Message* create(unsigned short id) const { return create_base(id); }
    virtual Message* clone() const;

    /// Return true if handled
    virtual bool run(MessageInterface* callback, unsigned id) = 0;

protected:
    Message(const Message& other) : msgId_(other.msgId_) {}

    Message& operator=(const Message& other)
    {
        if(this == &other)
            return *this;

        msgId_ = other.msgId_;

        return *this;
    }
};

inline Message* new_clone(const Message& msg)
{
    return msg.clone();
}

#endif //! MESSAGE_H_INCLUDED
