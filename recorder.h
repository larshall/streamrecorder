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
using std::ios;

#define MAX_BUFFER_SIZE 8024
// Number of frames to buffer before writing to disk
// Note: This needs be smaller than buffer - the MTU size
// (plus a little extra)
#define MAX_BUFFER_FRAMES MAX_BUFFER_SIZE / 2
// Reconnect interval in seconds
#define RECONNECT_INTERVAL 5

class Recorder : public Thread
{

    private:
        Mutex mutex;
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
        void cleanup();

    public:
        Recorder(const string &host, uint16_t port);
        ~Recorder();

        void run();

        void setStartTime(time_t startTime)
        {
            ScopedLock lock(&mutex);
            this->startTime = startTime;
        }

        time_t getStartTime()
        {
            ScopedLock lock(&mutex);
            return startTime;
        }

        void setEndTime(time_t endTime)
        {
            ScopedLock lock(&mutex);
            this->endTime = endTime;
        }

        time_t getEndTime()
        {
            ScopedLock lock(&mutex);
            return endTime;
        }

        void setTitle(const string &title)
        {
            ScopedLock lock(&mutex);
            this->title = title;
        }

        void setDescription(const string &description)
        {
            ScopedLock lock(&mutex);
            this->description = description;
        }

        void setFilename(const string &filename)
        {
            ScopedLock lock(&mutex);
            this->filename = filename;
        }

        uint64_t getPacketLoss()
        {
            ScopedLock lock(&mutex);
            return rtp.getPacketLoss();
        }
};

#endif
