#ifndef STREAMRECORDER_H
#define STREAMRECORDER_H

#include "xmltv.h"
#include "httpserver.h"
#include "thread.h"

#define DEBUG

class StreamRecorder
{
    private:
        int numRecordings;
        XmlTv xmltv;
        Mutex mutex;
    public:
        StreamRecorder();
        void getChannels(vector<Channel> &channels);
        void getProgrammes(const string &channelId,
            vector<Programme> &programmes);
        void getProgramme(const string &channelId,
            const string &start, Programme &programme);
};

#endif
