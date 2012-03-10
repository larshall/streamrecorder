/*
 * Copyright (C) 2012 Lars Hall
 *
 * This file is part of StreamRecorder.
 *
 * StreamRecorder is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * StreamRecorder is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with StreamRecorder.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RTP_H
#define RTP_H

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <string>

using std::string;

#define RTP_HEADER_SIZE 12
#define MTU_SIZE 1500
// Theoretical limit. See your MTU size (approx 1500)
#define MAX_UDP_SIZE 65535

struct RtpPacket
{
    uint8_t payload[MTU_SIZE];
    uint8_t type;
    uint16_t seqnum;
    uint8_t timestamp;
    uint16_t payloadLen;
};

class Rtp
{
    private:
        int fd;
        uint64_t packetLoss;
        uint16_t lastSeqnum;

        bool connected;
        struct sockaddr_in sock;

    public:
        Rtp();
        bool connect(const string &host, uint16_t port);
        bool receive();
        RtpPacket *readPacket();

        uint64_t getPacketLoss()
        {
            return packetLoss;
        }

};

#endif
