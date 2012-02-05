#include "recorder.h"

Recorder::Recorder(const string &host, uint16_t port)
{
    filename = "";
    description = "";
    startTime = time(NULL);
    endTime = time(NULL);
    buffer = new uint8_t[MAX_BUFFER_SIZE];
    bufferLen = 0;
    outputFile.open("./blah.x264");
    rtp.connect("239.255.0.4", 1234);
}

void Recorder::run()
{
    while(true)
    {
        if (doTerminate())
            break;
        fprintf(stderr, "\n---");
        RtpPacket *packet = rtp.readPacket();
        if (packet != NULL)
        {
            fprintf(stderr, "seqnum:%i\n", packet->seqnum);
            fprintf(stderr, "type:%i\n", packet->type);
            memcpy(buffer + bufferLen, packet->payload, packet->payloadLen);
            bufferLen += packet->payloadLen;

            if (bufferLen > 4000)
                writeBuffer();
#ifdef DEBUG
            H264::decodeNal(packet);
#endif
        }
    }

}

bool Recorder::writeBuffer()
{
    outputFile.write((char*)buffer, bufferLen);
    bufferLen = 0;
    return true;
}

Recorder::~Recorder()
{
    outputFile.close();
    delete [] buffer;
}
