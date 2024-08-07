// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "MessageHandler.h"
#include "Message.h"
#include "Socket.h"
#include "SocketSet.h"
#include "s25util/Log.h"
#include "s25util/MyTime.h"
#include "s25util/Serializer.h"
#include <boost/endian/arithmetic.hpp>
#include <cstring>
#include <stdexcept>

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

    const auto packetLen = ser.GetLength();
    unsigned char* data = ser.GetDataWritable(packetLen);
    header.msgId = msg.getId();
    header.msgLen = packetLen - sizeof(header);

    std::memcpy(data, &header, sizeof(header));

    if(packetLen != (unsigned)sock.Send(data, packetLen))
        return -1;
    else
        return packetLen;
}

/// Helper function to determine if the timeout is reached to be used in a loop. Updates lasttime and the remaining
/// timeout
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
    }

    Message* msg = createMsg(header.msgId);
    if(!msg)
        return nullptr;

    msg->Deserialize(ser);
    error = 0;
    return msg;
}
