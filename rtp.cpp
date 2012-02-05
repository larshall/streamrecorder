#include "rtp.h"

Rtp::Rtp()
{
    connected = false;
}

bool Rtp::connect(const string &host, uint16_t port)
{
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        fprintf(stderr, "Opening datagram socket error");
        exit(1);
    }
    else
        printf("Opening datagram socket....OK.\n");
 
    int reuse = 1;
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
        (char *)&reuse, sizeof(reuse)) < 0)
    {
        fprintf(stderr, "Setting SO_REUSEADDR error");
        close(fd);
        return false;
    }
 
    memset((char *) &sock, 0, sizeof(sock));
    sock.sin_family = AF_INET;
    sock.sin_port = htons(port);
    sock.sin_addr.s_addr = INADDR_ANY;
    if(bind(fd, (struct sockaddr*)&sock, sizeof(sock)))
    {
        fprintf(stderr, "Binding error");
        close(fd);

        return false;
    }
 
    group.imr_multiaddr.s_addr = inet_addr(host.c_str());

    if(setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
        (char *)&group, sizeof(group)) < 0)
    {
        fprintf(stderr, "Adding multicast group error");
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
        perror("Reading datagram message error");
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

    packet->payload = new uint8_t[numbytes - size];
    packet->payloadLen = numbytes-size;

    memcpy(packet->payload, databuf + size, numbytes - size);

    return packet;
}
