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

// Oooh why oh why
// Have to use UINT64_MAX
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include "rtp.h"

Rtp::Rtp()
{
    connected = false;
    fd = -1;
    packetLoss = 0;
}

bool Rtp::connect(const string &host, uint16_t port)
{
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        fprintf(stderr, "Error: socket\n");

        return false;
    }
    else
        printf("Opening datagram socket....OK.\n");
 
    int reuse = 1;
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
        (char *)&reuse, sizeof(reuse)) < 0)
    {
        fprintf(stderr, "Error: SO_REUSEADDR\n");
        close(fd);

        return false;
    }
 
    memset((char *) &sock, 0, sizeof(sock));
    sock.sin_family = AF_INET;
    sock.sin_port = htons(port);
    sock.sin_addr.s_addr = INADDR_ANY;
    if(bind(fd, (struct sockaddr*)&sock, sizeof(sock)))
    {
        fprintf(stderr, "Error: bind\n");
        close(fd);

        return false;
    }
 
    struct ip_mreq group;
    group.imr_multiaddr.s_addr = inet_addr(host.c_str());

    if(setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
        (char *)&group, sizeof(group)) < 0)
    {
        fprintf(stderr, "Error: adding multicast group\n");
        close(fd);

        return false;
    }
 
    connected = true;

    return true;
}

RtpPacket *Rtp::readPacket()
{
    if (!connected)
        return NULL;

    uint8_t databuf[MAX_UDP_SIZE];

    int datalen = sizeof(databuf);
    int numbytes = 0;
    if((numbytes = read(fd, databuf, datalen)) < 0)
    {
        perror("Error: reading rtp packet\n");
        connected = false;
        close(fd);
        return NULL;
    }

    RtpPacket *packet = new RtpPacket;
    packet->type = databuf[1] & 0x7f;

    packet->seqnum = (databuf[2] << 8) | databuf[3];

    packet->timestamp = (databuf[4] << 24) | (databuf[5] << 16) |
        (databuf[6] << 8) | databuf[7];

    unsigned int size = RTP_HEADER_SIZE;
    size += 4 * (databuf[0] & 0xf);
    if (databuf[0] & 0x10) // header extension
        size += 4 * (1 + (databuf[size + 2] << 8) + databuf[size + 3]);

    if ((lastSeqnum != 0) && (packetLoss < UINT64_MAX))
        packetLoss += packet->seqnum - lastSeqnum;

    lastSeqnum = packet->seqnum;

    packet->payloadLen = numbytes - size;
    memcpy(packet->payload, databuf + size, numbytes - size);

    return packet;
}
