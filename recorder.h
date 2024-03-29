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

        string getStartTimeStr()
        {
            ScopedLock lock(&mutex);
            char buf[20];
            strftime(buf, 20, "%Y-%m-%d %H:%M:%S", localtime(&startTime));
            return string(buf);
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

        string getEndTimeStr()
        {
            ScopedLock lock(&mutex);
            char buf[20];
            strftime(buf, 20, "%Y-%m-%d %H:%M:%S", localtime(&endTime));
            return string(buf);
        }

        void setTitle(const string &title)
        {
            ScopedLock lock(&mutex);
            this->title = title;
        }

        string getTitle()
        {
            ScopedLock lock(&mutex);
            return title;
        }

        string getDescription()
        {
            ScopedLock lock(&mutex);
            return description;
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
