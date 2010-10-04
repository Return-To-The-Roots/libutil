// $Id: Message.cpp 6582 2010-07-16 11:23:35Z FloSoft $
//
// Copyright (c) 2005 - 2010 Settlers Freaks (sf-team at siedler25.org)
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
#include "main.h"
#include "Message.h"

#include "Messages.h"

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author FloSoft
 */
bool Message::send(Socket *sock)
{
	static char buffer[1024001];
		
	if(NULL == sock)
		return false;

	// WTF? so große Nachricht darfs nicht geben
	if(GetLength() > 1024000 - 6)
	{
		LOG.lprintf("BIG OOPS! Message with length %u exceeds maximum of %d!\n", GetLength(), 1024000 - 6);
		return false;
	}

	unsigned short *id = (unsigned short*)&buffer[0];
	unsigned int *length = (unsigned int*)&buffer[2];
	unsigned char *data = (unsigned char*)&buffer[6];

	*id = this->id;
	*length = GetLength();
	memcpy(data, GetData(), GetLength());

	if(6 + GetLength() != (unsigned int)sock->Send(buffer, 6 + GetLength()))
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liest vom Socket die Paketdatenmenge
 *
 *  @author FloSoft
 */
int Message::recv(Socket *sock, unsigned int length)
{
	if(length > 0)
	{
		// Daten empfangen
		Realloc(length);

		int read = sock->Recv(GetDataWritable(), length);
		SetLength(length);
		if(length != (unsigned int)read )
		{
			LOG.lprintf("recv: data: only got %d bytes instead of %d\n", read, length);

			return -1;
		}
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author FloSoft
 */
Message *Message::create_base(unsigned short id)
{
	Message *msg = NULL;
	switch(id)
	{
	default: printf("ERROR: Message with id %u couldn't be created!\n",unsigned(id)); break;

	case NMS_NULL_MSG:	{ msg = new Message_Null(); } break;
	case NMS_DEAD_MSG:	{ msg = new Message_Dead(); } break;
	}

	return msg;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author FloSoft
 */
Message *Message::recv(Socket *sock, int &error, bool wait, Message *(*createfunction)(unsigned short))
{
	error = -1;

	if(sock == NULL)
		return NULL;

	unser_time_t time = TIME.CurrentTick();

	unser_time_t timeout = time;
	unsigned int received;
	SocketSet set;
 
	while(true)
	{
		// Warten wir schon 5s auf Antwort?
		if(time - timeout > 15000)
			wait = false;

		time = TIME.CurrentTick();

		// SocketSet "saubermachen"
		set.Clear();

		// Socket hinzufgen
		set.Add(*sock);

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
		if(!set.InSet(*sock) || sock->BytesWaiting(&received) != 0)
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

	int read = -1;

	char block[6];
	unsigned short *id = (unsigned short*)&block[0];
	unsigned int *length = (unsigned int*)&block[2];

	// block empfangen
	read = sock->Recv(block, 6, false);
	if(read != 6)
	{
		LOG.write("recv: block: only got %d bytes instead of %d, waiting for next try\n", read, 6);
		if(read != -1)
			error = 3;
		
		return NULL;
	}
	
	read = sock->BytesWaiting();

	static unsigned int blocktimeout = 0;
	if(read < (signed)((*length) + 6) )
	{
		++blocktimeout;
		LOG.write("recv: block-waiting: not enough input (%d/%d) for message (0x%04X), waiting for next try\n", read, (*length)+6, *id);
		if(blocktimeout < 120 && read != -1)
			error = 4;
		
		return NULL;
	}
	blocktimeout = 0;

	// Block nochmals abrufen (um ihn aus dem Cache zu entfernen)
	read = sock->Recv(block, 6);
	if(read != 6)
	{
		LOG.lprintf("recv: id,length: only got %d bytes instead of %d\n", read, 2);
		return NULL;
	}

	Message *msg = createfunction(*id);
	if(!msg)
		return NULL;

	// Daten abrufen
	msg->recv(sock, *length);

	return msg;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  dupliziert eine Nachricht.
 *
 *  @author FloSoft
 */
Message *Message::duplicate(void) const
{
	Message *msg = create(id);

	*msg = *this;

	return msg;
}
