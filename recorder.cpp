#include "recorder.h"

Recorder::Recorder(const string &host, uint16_t port)
{
    filename = "";
    description = "";
    startTime = time(NULL);
    endTime = time(NULL);
    buffer = new uint8_t[MAX_BUFFER_SIZE];
    bufferLen = 0;
    connected = false;
}

bool Recorder::connect()
{
    return rtp.connect(host.c_str(), port);
}

void Recorder::run()
{
    while(true)
    {

        if (doTerminate())
            break;
 
        if (!connected)
        {
            connected = connect();
            outputFile.open("./blah.x264");
            if (!connected)
            {
                // Only try once
                fprintf(stderr, "Cannot connect to:%s", host.c_str());
                break;
            }
        }

        fprintf(stderr, "\n---");
        RtpPacket *packet = rtp.readPacket();
        if (packet != NULL)
        {
            fprintf(stderr, "seqnum:%i\n", packet->seqnum);
            fprintf(stderr, "type:%i\n", packet->type);
            memcpy(buffer + bufferLen, packet->payload, packet->payloadLen);
            bufferLen += packet->payloadLen;

            if (bufferLen > MAX_BUFFER_SIZE)
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
