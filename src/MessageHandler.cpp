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

#include "MessageHandler.h"
#include "Log.h"
#include "Message.h"
#include "MyTime.h"
#include "Serializer.h"
#include "Socket.h"
#include "SocketSet.h"
#include <boost/endian/arithmetic.hpp>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <stdint.h>

struct MsgHeader
{
    // Use little endian here for backwards compatibility. TODO: Use correct order
    boost::endian::little_uint16_t msgId;
    boost::endian::little_int32_t msgLen;
};

int MessageHandler::send(Socket& sock, const Message& msg)
{
    MsgHeader header;

    Serializer ser;
    ser.PushRawData(&header, sizeof(header)); // Updated later
    msg.Serialize(ser);

    if(ser.GetLength() > 64 * 1024) /// 64KB, ACHTUNG: IPV4 garantiert nur maximal 576!!
    {
        LOG.write("BIG OOPS! Message with length %u exceeds maximum of %d!\n") % ser.GetLength() % (64 * 1024);
        return -1;
    }

    unsigned char* data = ser.GetDataWritable(ser.GetLength());
    header.msgId = msg.getId();
    header.msgLen = ser.GetLength() - sizeof(header);

    std::memcpy(data, &header, sizeof(header));

    if(ser.GetLength() != (unsigned)sock.Send(data, ser.GetLength()))
        return -1;
    else
        return ser.GetLength();
}

/// Helper function to determine if the timeout is reached to be used in a loop. Updates lasttime and the remaining timeout
static inline bool timeoutReached(s25util::time64_t& lastTime, unsigned& remainingTimeoutInMs)
{
    if(remainingTimeoutInMs == 0u)
        return true;

    s25util::time64_t now = s25util::Time::CurrentTick();
    s25util::time64_t elapsed = now - lastTime;
    lastTime = now;
    if(elapsed >= remainingTimeoutInMs)
    {
        remainingTimeoutInMs = 0;
        return true;
    } else
    {
        remainingTimeoutInMs = static_cast<unsigned>(remainingTimeoutInMs - elapsed);
        return false;
    }
}

Message* MessageHandler::recv(Socket& sock, int& error, unsigned timeoutInMs)
{
    error = -1;

    s25util::time64_t lastTime = s25util::Time::CurrentTick();
    MsgHeader header;
    do
    {
        SocketSet set;
        // Socket hinzufgen
        set.Add(sock);

        // liegen Daten an?
        int retval = set.Select(timeoutInMs, 0);

        if(retval <= 0)
        {
            if(timeoutInMs)
                continue;

            if(retval != -1)
                error = 0;

            return nullptr;
        }

        // liegen diese Daten an unserem Socket, bzw wieviele Bytes liegen an?
        unsigned numBytesAv;
        if(!set.InSet(sock) || sock.BytesWaiting(&numBytesAv) != 0)
        {
            if(timeoutInMs)
                continue;

            error = 1;
            return nullptr;
        }

        // socket ist geschlossen worden
        if(numBytesAv == 0)
            return nullptr;

        // haben wir schon eine vollständige nachricht?
        if(numBytesAv < sizeof(header))
        {
            if(timeoutInMs)
                continue;

            error = 2;
            return nullptr;
        }
        break;
    } while(!timeoutReached(lastTime, timeoutInMs));

    // block empfangen
    int read = sock.Recv(&header, sizeof(header), false);
    if(read != sizeof(header))
    {
        LOG.write("recv: block: only got %d bytes instead of %d, waiting for next try\n") % read % sizeof(header);
        if(read != -1)
            error = 3;

        return nullptr;
    }

    if(header.msgLen < 0)
        throw std::runtime_error("Integer overflow during recv of message");

    do
    {
        read = sock.BytesWaiting();
        if(read < 0)
            return nullptr;
        if(read >= static_cast<int>(header.msgLen + sizeof(header)) || !timeoutInMs)
            break;
        // Wait for socket again (non-blocking)
        SocketSet set;
        set.Add(sock);
        if(set.Select(0, 0) < 0)
            break;
    } while(!timeoutReached(lastTime, timeoutInMs));

    static unsigned blocktimeout = 0;
    if(read < static_cast<int>(header.msgLen + sizeof(header)))
    {
        ++blocktimeout;
        if(blocktimeout < 120)
            error = 4;

        return nullptr;
    }
    blocktimeout = 0;

    // Block nochmals abrufen (um ihn aus dem Cache zu entfernen)
    read = sock.Recv(&header, sizeof(header));
    if(read != sizeof(header))
    {
        LOG.write("recv: id,length: only got %d bytes instead of %d\n") % read % sizeof(header);
        return nullptr;
    }

    Serializer ser;
    if(header.msgLen)
    {
        read = sock.Recv(ser.GetDataWritable(header.msgLen), header.msgLen);
        if(read < header.msgLen)
        {
            LOG.write("recv: data: only got %d bytes instead of %d\n") % read % header.msgLen;
            return nullptr;
        }
        ser.SetLength(header.msgLen);
    }

    Message* msg = createMsg(header.msgId);
    if(!msg)
        return nullptr;

    msg->Deserialize(ser);
    error = 0;
    return msg;
}
