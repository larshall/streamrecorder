#ifndef STREAMRECORDER_H
#define STREAMRECORDER_H

#include "settings.h"
#include "xmltv.h"
#include "httpserver.h"
#include "thread.h"
#include "recorder.h"

#define DEBUG
// Probably not soo many recordings at the same time :)
#define MAX_RECORDERS 100
// Reaps every REAP_INTERVAL seconds
#define REAP_INTERVAL 30

class StreamRecorder
{
    private:
        // every recording have a corresponding Recorder
        // (So every programme gets it's own recorder thread)
        list<Recorder*> recorders;
        XmlTv xmltv;
        Settings settings;
        Mutex mutex;
        /// the time of last reap
        time_t lastReap;
        /// Reaping finished recordings
        void reapRecorders();
    public:
        StreamRecorder();
        void process();
        void getChannels(vector<Channel> &channels);
        void getProgrammes(const string &channelId,
            vector<Programme> &programmes);
        void getProgramme(const string &channelId,
            const string &start, Programme &programme);
        void record(const string &channelId, const string &start);
        void saveChannelStream(const string &channel, const string &host,
            const string &port);
        void getChannelStreams(vector<ChannelStream> &streams);
        void deleteChannelStream(const string &channel);
};

#endif
