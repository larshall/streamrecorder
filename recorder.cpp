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
    this->host = host;
    this->port = port;
}

void Recorder::run()
{
    while(true)
    {

        if (doTerminate())
            break;
 
        if (!connected)
        {
            connected = rtp.connect(host.c_str(), port);
            outputFile.open(filename.c_str(), ios::trunc | ios::out);
            if (!connected)
            {
                fprintf(stderr, "Cannot connect to:%s:%i\n",
                    host.c_str(), port);
                sleep(RECONNECT_INTERVAL);
            }
        }

        RtpPacket *packet = rtp.readPacket();
        if (packet != NULL)
        {
            // TODO: Add an opstion for debug info
            //fprintf(stderr, "seqnum:%i\n", packet->seqnum);
            //fprintf(stderr, "type:%i\n", packet->type);
            memcpy(buffer + bufferLen, packet->payload, packet->payloadLen);
            bufferLen += packet->payloadLen;

            if (bufferLen > MAX_BUFFER_FRAMES)
                writeBuffer();
#ifdef DEBUG
            H264::decodeNal(packet);
#endif
        }

        usleep(0.1);
    }

    cleanup();
}

bool Recorder::writeBuffer()
{
    outputFile.write((char*)buffer, bufferLen);
    bufferLen = 0;

    return true;
}

void Recorder::cleanup()
{
    if (bufferLen > 0)
        writeBuffer();

    if (outputFile.is_open())
    {
        outputFile.flush();
        outputFile.close();
    }
}

Recorder::~Recorder()
{
    cleanup();
    delete [] buffer;
}
