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
