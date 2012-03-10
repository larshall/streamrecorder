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

