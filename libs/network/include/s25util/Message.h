// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cstdint>

class MessageInterface;
class Serializer;

class Message
{
    uint16_t msgId_;

public:
    Message(uint16_t id) : msgId_(id) {}
    virtual ~Message() = default;

    virtual void Serialize(Serializer& /*ser*/) const {}
    virtual void Deserialize(Serializer& /*ser*/) {}

    unsigned short getId() const { return msgId_; }
    static Message* create_base(unsigned short id);

    virtual Message* create(unsigned short id) const { return create_base(id); }
    virtual Message* clone() const;

    /// Return true if handled
    virtual bool run(MessageInterface* callback, unsigned id) = 0;

protected:
    Message(const Message& other) = default;

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
