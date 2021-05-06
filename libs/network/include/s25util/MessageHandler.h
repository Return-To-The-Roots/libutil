// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

class Message;
class Socket;

using CreateMsgFunction = Message* (*)(unsigned short);

class MessageHandler
{
    CreateMsgFunction createMsg;

public:
    MessageHandler(CreateMsgFunction createMsg) : createMsg(createMsg) {}

    /// Sends that message. Returns number of bytes submitted (-1 for failure)
    static int send(Socket& sock, const Message& msg);
    /// Receives a message. Returns nullptr on error, or if nothing is available
    /// Sets error: -1 = Fatal Error, 0 = socket busy, 1 = No msg available, 2 = incomplete, 3 = incomplete header, 4 =
    /// incomplete data Blocks for about timeoutInMs ms if message is not available
    Message* recv(Socket& sock, int& error, unsigned timeoutInMs);
};
