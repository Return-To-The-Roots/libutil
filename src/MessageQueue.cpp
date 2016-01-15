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
#include "MessageQueue.h"
#include "Message.h"
#include "Socket.h"
#include "Log.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p MessageQueue
 *
 *  @author FloSoft
 */
MessageQueue::~MessageQueue(void)
{
    clear();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Copy-Konstruktor von @p MessageQueue, um Messages neu zu erstellen, ansonsten werden
 *  nur die Pointer kopiert!
 *
 *  @author OLiver
 */
MessageQueue::MessageQueue(const MessageQueue& mq): MessageHandler(mq), messages(mq.messages)
{
    // Deep copy
    for(QueueIt it = messages.begin(); it != messages.end(); ++it)
        *it = (*it)->duplicate();
}

/// Zuweisungsoperator, da Messages kopiert werden müssen
MessageQueue& MessageQueue::operator=(const MessageQueue& mq)
{
    if(this == &mq)
        return *this;

    MessageHandler::operator=(mq);

    clear();
    messages = mq.messages;

    // Deep copy
    for(QueueIt it = messages.begin(); it != messages.end(); ++it)
        *it = (*it)->duplicate();

    return *this;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  entfernt alle Elemente aus der Queue.
 *
 *  @author FloSoft
 */
void MessageQueue::clear(void)
{
    while(!empty())
        pop();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  entfernt das vorderste Element aus der Queue.
 *
 *  @author FloSoft
 */
void MessageQueue::pop(void)
{
    if(messages.empty())
        return;

    delete messages.front();
    messages.pop();
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

///////////////////////////////////////////////////////////////////////////////
/*
 *
 *
 *  @author FloSoft
 */
bool MessageQueue::send(Socket& sock, int max, unsigned int sizelimit)
{
    if(!sock.isValid())
        return false;

    // send-queue abarbeiten
    int count = 0;
    while(count <= max && !messages.empty())
    {
        const Message& msg = *messages.front();        

        if(msg.getId() > 0)
        {
            int sendBytes = MessageHandler::send(sock, msg);
            if(sendBytes < 0)
            {
                LOG.lprintf("Sending Message to server failed\n");
                return false;
            }else if(static_cast<unsigned>(sendBytes) > sizelimit)
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
