#include "h264.h"

H264::H264()
{
}

// TODO: Add support for h264 fragmentation
// Note: this doesn't do anything other than
// checking that the h.264 nals aren't fragmented
bool H264::decodeNal(RtpPacket *packet)
{
    bool ret = true;
    int type = (uint8_t)packet->payload[0] & 0x1f;

    if (type >= 1 && type <= 23)
        fprintf(stderr, "Nal unit type:%i\n", type);
    else
    {
        // Not supported
        fprintf(stderr, "Fragmentationi type:%i\n", type);
        ret = false;
    }

#ifdef DEBUG
    printf("\nnaltype:%i", packet->payload[0] & 0x1f);
    fprintf(stderr, "Forbidden bit:%i\n", (packet->payload[0]>>7) & 1);
    fprintf(stderr, "Ref idc:%i\n", ((packet->payload[0]>>5) & 3));
#endif

    return ret;
}

