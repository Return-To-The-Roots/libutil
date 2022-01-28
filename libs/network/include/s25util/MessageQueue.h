// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Message.h"
#include "MessageHandler.h"
#include <boost/ptr_container/ptr_deque.hpp>
#include <cstddef>
#include <deque>
#include <memory>
#include <queue>

class Socket;

// class Socket;
class MessageQueue : protected MessageHandler
{
public:
    explicit MessageQueue(CreateMsgFunction createfunction);
    ~MessageQueue();

    void clear();

    /// flusht die Queue, verschickt alle Elemente.
    bool flush(Socket& sock);

    /// liefert die Größe der Queue
    size_t size() const;
    bool empty() const;

    /// verschickt Pakete der Queue, maximal @p max, mit einem maximal @p sizelimit groß (aber beliebig viele kleine)
    bool send(Socket& sock, int max, unsigned sizelimit = 512);
    /// Receive (at most) a single message.
    /// If timeout is 0 then the function returns immediately after checking for a message
    /// Otherwise it keeps waiting for about that timeout till a complete message is received
    /// Returns the number of messages received or -1 on error
    int recv(Socket& sock, unsigned timeoutInMs = 0);
    /// Receive all complete messages. The timeout specifies the timeout for the first message (see recv)
    /// Returns the number of messages received or -1 on error
    int recvAll(Socket& sock, unsigned timeoutInMs = 0);
    /// Sends a message directly
    static bool sendMessage(Socket& sock, const Message& msg) { return MessageHandler::send(sock, msg) >= 0; }

    /// hängt ein Element hinten an.
    void push(Message* message);
    /// liefert das vorderste Element der Queue.
    Message* peek();
    /// entfernt das vorderste Element aus der Queue.
    std::unique_ptr<Message> pop();

private:
    boost::ptr_deque<Message> messages;
};
