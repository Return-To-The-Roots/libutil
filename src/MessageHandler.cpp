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

#include "libUtilDefines.h"
#include "MessageHandler.h"
#include "Message.h"
#include "Socket.h"
#include "SocketSet.h"
#include "Log.h"
#include "MyTime.h"
#include <boost/endian/conversion.hpp>

int MessageHandler::send(Socket& sock, const Message& msg)
{
    Serializer ser;
    ser.PushUnsignedShort(0); // Id
    ser.PushSignedInt(0); // Placeholder for length
    const unsigned headerSize = ser.GetLength();
    msg.Serialize(ser);

    if(ser.GetLength() > 64 * 1024) /// 64KB, ACHTUNG: IPV4 garantiert nur maximal 576!!
    {
        LOG.lprintf("BIG OOPS! Message with length %u exceeds maximum of %d!\n", ser.GetLength(), 64 * 1024);
        return -1;
    }

    unsigned char* data = ser.GetDataWritable(ser.GetLength());
    // Use little endian here for backwards compatibility
    *reinterpret_cast<int*>(data) = boost::endian::native_to_little(msg.getId());
    *reinterpret_cast<int*>(data + sizeof(unsigned short)) = boost::endian::native_to_little(ser.GetLength() - headerSize);

    if(ser.GetLength() != (unsigned int)sock.Send(data, ser.GetLength()))
        return -1;
    else
        return ser.GetLength();
}

Message* MessageHandler::recv(Socket& sock, int& error, bool wait)
{
    error = -1;

    unser_time_t startTime = TIME.CurrentTick();

    while(true)
    {
        // Warten wir schon 15s auf Antwort?
        if(wait && TIME.CurrentTick() - startTime > 15000)
            wait = false;

        SocketSet set;
        // Socket hinzufgen
        set.Add(sock);

        // liegen Daten an?
        int retval = set.Select(0, 0);

        if(retval <= 0)
        {
            if(wait)
                continue;

            if(retval != -1)
                error = 0;

            return NULL;
        }

        // liegen diese Daten an unserem Socket, bzw wieviele Bytes liegen an?
        unsigned int received;
        if(!set.InSet(sock) || sock.BytesWaiting(&received) != 0)
        {
            if(wait)
                continue;

            error = 1;
            return NULL;
        }

        // socket ist geschlossen worden
        if(received == 0)
            return NULL;

        // haben wir schon eine vollständige nachricht? (kleinste nachricht: 6 bytes)
        if(received < 6)
        {
            if(wait)
                continue;

            error = 2;
            return NULL;
        }
        break;
    }

    char header[6];

    // block empfangen
    int headerSize = sizeof(header);
    int read = sock.Recv(header, headerSize, false);
    if(read != headerSize)
    {
        LOG.write("recv: block: only got %d bytes instead of %d, waiting for next try\n", read, headerSize);
        if(read != -1)
            error = 3;

        return NULL;
    }

    // Those are little endian for backwards compatibility
    unsigned short id = boost::endian::little_to_native(*reinterpret_cast<unsigned short*>(&header[0]));
    int length = boost::endian::little_to_native(*reinterpret_cast<int*>(&header[sizeof(unsigned short)]));

    if(length < 0)
        throw std::runtime_error("Integer overflow during recv of message");

    read = sock.BytesWaiting();

    static unsigned blocktimeout = 0;
    if(read < length + headerSize)
    {
        ++blocktimeout;
        LOG.write("recv: block-waiting: not enough input (%d/%d) for message (0x%04X), waiting for next try\n", read, length + headerSize, id);
        if(blocktimeout < 120 && read != -1)
            error = 4;

        return NULL;
    }
    blocktimeout = 0;

    // Block nochmals abrufen (um ihn aus dem Cache zu entfernen)
    read = sock.Recv(header, headerSize);
    if(read != headerSize)
    {
        LOG.lprintf("recv: id,length: only got %d bytes instead of %d\n", read, 2);
        return NULL;
    }

    Serializer ser;
    if(length)
    {
        read = sock.Recv(ser.GetDataWritable(length), length);
        if(read < length)
        {
            LOG.lprintf("recv: data: only got %d bytes instead of %d\n", read, length);
            return NULL;
        }
        ser.SetLength(length);
    }

    Message* msg = createMsg(id);
    if(!msg)
        return NULL;

    msg->Deserialize(ser);

    return msg;
}