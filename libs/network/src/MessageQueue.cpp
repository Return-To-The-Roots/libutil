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

#include "MessageQueue.h"
#include "s25util/Log.h"
#include "Message.h"
#include "Socket.h"
#include <limits>

MessageQueue::MessageQueue(CreateMsgFunction createfunction) : MessageHandler(createfunction) {}

MessageQueue::~MessageQueue() = default;

/**
 *  entfernt alle Elemente aus der Queue.
 */
void MessageQueue::clear()
{
    messages.clear();
}

void MessageQueue::push(Message* message)
{
    messages.push_back(message);
}

Message* MessageQueue::front()
{
    return (!messages.empty() ? &messages.front() : nullptr);
}

/**
 *  entfernt das vorderste Element aus der Queue.
 */
void MessageQueue::pop()
{
    if(!messages.empty())
        messages.pop_front();
}

Message* MessageQueue::popFront()
{
    if(messages.empty())
        return nullptr;
    return messages.pop_front().release();
}

int MessageQueue::recv(Socket& sock, unsigned timeoutInMs)
{
    // Nachricht abrufen
    int error = -1;
    Message* msg = MessageHandler::recv(sock, error, timeoutInMs);

    if(msg)
    {
        push(msg);
        return 1;
    } else
        return error < 0 ? -1 : 0;
}

int MessageQueue::recvAll(Socket& sock, unsigned timeoutInMs)
{
    // Receive first msg
    int result = recv(sock, timeoutInMs);
    // If any received get also all remaining without further waiting
    if(result > 0)
    {
        while(recv(sock) > 0)
            result++;
    }
    return result;
}

bool MessageQueue::flush(Socket& sock)
{
    assert(messages.size() < static_cast<size_t>(std::numeric_limits<int>::max()));
    return send(sock, static_cast<int>(messages.size()), std::numeric_limits<unsigned>::max());
}

size_t MessageQueue::size() const
{
    return messages.size();
}

bool MessageQueue::empty() const
{
    return messages.empty();
}

bool MessageQueue::send(Socket& sock, int max, unsigned sizelimit)
{
    if(!sock.isValid())
        return false;

    // send-queue abarbeiten
    int count = 0;
    while(count <= max && !messages.empty())
    {
        const Message& msg = *front(); //-V522

        if(msg.getId() > 0)
        {
            int sendBytes = MessageHandler::send(sock, msg);
            if(sendBytes < 0)
            {
                LOG.write("Sending Message to server failed\n");
                return false;
            } else if(static_cast<unsigned>(sendBytes) > sizelimit)
            {
                pop();
                break; // maximal 1 großes Paket verschicken
            }
        }

        pop();

        ++count;
    }
    return true;
}
