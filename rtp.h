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
    uint8_t *payload;
    uint8_t type;
    uint16_t seqnum;
    uint8_t timestamp;
    uint16_t payloadLen;
};

class Rtp
{
    private:
        int fd;
        bool connected;
        struct sockaddr_in sock;
        struct ip_mreq group;

    public:
        Rtp();
        bool connect(const string &host, uint16_t port);
        bool receive();
        RtpPacket *readPacket();

};

#endif
