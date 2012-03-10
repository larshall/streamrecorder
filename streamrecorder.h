#ifndef STREAMRECORDER_H
#define STREAMRECORDER_H

#include <limits.h>
#include "settings.h"
#include "xmltv.h"
#include "httpserver.h"
#include "thread.h"
#include "recorder.h"

#define DEFAULT_XMLTV_PATH "./data"
#define DEFAULT_DATADIR_PATH "./data"
#define DEFAULT_HTTPSERVER_PATH "./data"
#define DEFAULT_WEBSERVER_PORT 8182

#define DEBUG
// Probably not soo many recordings at the same time :)
#define MAX_RECORDERS 100
// Reaps every REAP_INTERVAL seconds
#define REAP_INTERVAL 30

void showUsage();

class StreamRecorder
{
    private:
        /// every recording have a corresponding Recorder
        /// (So every programme gets it's own recorder thread)
        list<Recorder*> recorders;
        XmlTv xmltv;
        Settings settings;
        Mutex mutex;
        /// the time of the last reap
        time_t lastReap;
        /// Reaps finished recordings
        void reapRecorders();
    public:
        StreamRecorder();
        void process();
        /// Gets channels (created channelstreams)
        void getChannels(vector<Channel> &channels);
        void getProgrammes(const string &channelId,
            vector<Programme> &programmes, const string &date);
        void getProgramme(const string &channelId,
            const string &start, Programme &programme);
        /// Records thre request programme/channel
        /// If Channelstream isn't found false is returned
        bool record(const string &channelId, const string &start);
        void saveChannelStream(const string &channel, const string &host,
            const string &port);
        void getChannelStreams(vector<ChannelStream> &streams);
        void deleteChannelStream(unsigned int channelId);
};

#endif
