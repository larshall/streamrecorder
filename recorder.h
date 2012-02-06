#ifndef RECORDER_H
#define RECORDER_H

#include <string>
#include <time.h>
#include <fstream>
#include "streamrecorder.h"
#include "rtp.h"
#include "h264.h"
#include "thread.h"

using std::string;
using std::ofstream;

#define MAX_BUFFER_SIZE 4024

class Recorder : public Thread
{
    public:
        // TODO
        enum RecordingType
        {
            REOCURRENT,
            SINGLE
        };

    private:
        Rtp rtp;
        H264 h264;
        ofstream outputFile;

        bool recording;
        time_t startTime;
        time_t endTime;
        string description;
        string filename;
        bool connected;
        uint8_t *buffer;
        int bufferLen;
        bool writeBuffer();

    public:
        Recorder(const string &host, uint16_t port);
        ~Recorder();
        void run();
};

#endif
