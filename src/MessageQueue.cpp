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

#include "libUtilDefines.h" // IWYU pragma: keep
#include "MessageQueue.h"
#include "Log.h"
#include "Message.h"
#include "Socket.h"
#include <limits>

MessageQueue::MessageQueue(CreateMsgFunction createfunction) : MessageHandler(createfunction) {}

MessageQueue::MessageQueue(const MessageQueue& other) : MessageHandler(other), messages(other.messages) {}

MessageQueue::~MessageQueue()
{
    clear();
}

MessageQueue& MessageQueue::operator=(const MessageQueue& other)
{
    MessageHandler::operator=(other);
    messages = other.messages;
    return *this;
}

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
    return (!messages.empty() ? &messages.front() : NULL);
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
        return NULL;
    return messages.pop_front().release();
}

///////////////////////////////////////////////////////////////////////////////
/// ruft eine nachricht ab und hängt sie in die queue
bool MessageQueue::recv(Socket& sock, bool wait)
{
    // Nachricht abrufen
    int error = -1;
    Message* msg = MessageHandler::recv(sock, error, wait);

    if(msg)
    {
        push(msg);
        return true;
    }

    // noch nicht alles empfangen, true liefern für okay (error == -1 bedeutet fehler)
    return (error >= 0);
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
