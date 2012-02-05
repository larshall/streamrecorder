#include "h264.h"

H264::H264()
{
}

// TODO: Add support for h264 fragmentation
// (Note: this doesn't do anything other than
// printing info about  the h.264 packet until h264 fragmentation
// is implemented)
void H264::decodeNal(RtpPacket *packet)
{

    printf("\nnaltype:%i", packet->payload[0] & 0x1f);

    int type = (uint8_t)packet->payload[0] & 0x1f;

    if (type >= 1 && type <= 23)
        fprintf(stderr, "Nal unit type:%i\n", type);
    else
        fprintf(stderr, "Fragmentationi type:%i\n", type);

    fprintf(stderr, "Forbidden bit:%i\n", (packet->payload[0]>>7) & 1);
    fprintf(stderr, "Ref idc:%i\n", ((packet->payload[0]>>5) & 3));
}

