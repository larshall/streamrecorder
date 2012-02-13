#ifndef RECORDER_H
#define RECORDER_H

#include <string>
#include <time.h>
#include <fstream>
#include "rtp.h"
#include "h264.h"
#include "thread.h"

using std::string;
using std::ofstream;

#define MAX_BUFFER_SIZE 4024

class Recorder : public Thread
{

    private:
        Rtp rtp;
        H264 h264;
        ofstream outputFile;
        bool connected;
        uint8_t *buffer;
        int bufferLen;

        string host;
        uint16_t port;
        bool recording;

        string title;
        string description;
        string filename;
        time_t startTime;
        time_t endTime;


        bool writeBuffer();
        bool connect();

    public:
        Recorder(const string &host, uint16_t port);
        ~Recorder();

        void run();

        void setStartTime(time_t startTime)
        {
            this->startTime = startTime;
        }

        void setEndTime(time_t endTime)
        {
            this->endTime = endTime;
        }

        void setTitle(const string &title)
        {
            this->title = title;
        }

        void setDescription(const string &description)
        {
            this->description = description;
        }

        void setFilename(const string &description)
        {
            this->filename = filename;
        }
};

#endif
