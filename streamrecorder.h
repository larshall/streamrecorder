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

#ifndef STREAMRECORDER_H
#define STREAMRECORDER_H

#include <limits.h>
#include "settings.h"
#include "xmltv.h"
#include "httpserver.h"
#include "thread.h"
#include "recorder.h"

#define DEFAULT_XMLTV_FILE "./data/tv.xml"
#define DEFAULT_DATADIR_PATH "./data"
#define DEFAULT_HTTPSERVER_PATH "./data"
#define DEFAULT_WEBSERVER_PORT 8182

#define DEBUG
// Probably not soo many recordings at the same time :)
#define MAX_RECORDERS 100
// Reaps every REAP_INTERVAL seconds
#define REAP_INTERVAL 30

void showUsage();

struct RecorderInfo
{
    string startTime;
    string endTime;
    string filename;
};

class StreamRecorder
{
    private:
        /// Every recording have a corresponding Recorder
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
        StreamRecorder(const string &xmltvFile);
        void process();
        /// Gets channels (created channelstreams)
        void getChannels(vector<Channel> &channels);
        void getProgrammes(const string &channelId,
            vector<Programme> &programmes, const string &date);
        void getProgramme(const string &channelId,
            const string &start, Programme &programme);
        /// Records the requested programme/channel
        /// If Channelstream isn't found, false is returned
        bool record(const string &channelId, const string &start);
        void saveChannelStream(const string &channel, const string &host,
            const string &port);
        void getChannelStreams(vector<ChannelStream> &streams);
        void getRecordings(vector<RecorderInfo> &recordings);
        void deleteChannelStream(unsigned int channelId);
};

#endif
