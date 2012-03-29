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

#include "rtp.h"
#include "h264.h"
#include "streamrecorder.h"
#include "webfrontend.h"

int main(int argc, char *argv[])
{
    int c;
    bool ok = true;
    char *optp = NULL;
    char *optx = NULL;
    char *opti = NULL;
    char *optd = NULL;

    string xmltvFile = DEFAULT_XMLTV_FILE;
    string datadirPath = DEFAULT_DATADIR_PATH;
    string httpPath = DEFAULT_HTTPSERVER_PATH;
    uint16_t webserverPort = DEFAULT_WEBSERVER_PORT;

    while((c = getopt(argc, argv, "p:h::x:i")) != -1)
    {
        switch(c)
        {
            case 'd':
                optd = optarg;
                break;
            case 'p':
                optp = optarg;
                break;
            case 'i':
                opti = optarg;
                break;
            case 'x':
                optx = optarg;
                break;
            case 'h':
                ok = false;
                break;
        }
    }
    
    if (optp != NULL)
    {
        int tmpport = atoi(optp);

        if ((optp != NULL) && (tmpport <= USHRT_MAX) && (tmpport > 0))
            webserverPort = (uint16_t)tmpport;
    }

    if (optd != NULL)
        datadirPath = optd;

    if (optx != NULL)
        xmltvFile = optx;

    if (ok)
    {
        StreamRecorder streamRecorder(xmltvFile);
        WebFrontend frontend(&streamRecorder);

        HttpServer server(webserverPort, httpPath, &frontend);
        server.start();
        while(true)
        {
            streamRecorder.process();
            // once a second is enough for starting stopping recordings
            sleep(1);
        }
    }
    else
        showUsage();
}

void showUsage()
{
    fprintf(stdout, "\nUsage: streamrecorder [-x xmltv path] [-d datadir] ");
    fprintf(stdout, "[-p webserverport] [-i httpserver path]\n");
}

StreamRecorder::StreamRecorder(const string &xmltvFile)
{
    xmltv.loadFile(xmltvFile, "");
    settings.load("data/settings.xml");
    lastReap = time(NULL);
}

void StreamRecorder::process()
{
    ScopedLock lock(&mutex);
    list<Recorder*>::iterator it;

    for (it = recorders.begin(); it != recorders.end(); it++)
    {
        if (!(*it)->isRunning())
        {
#ifdef DEBUG
            fprintf(stderr, "startTime:%i, end:%i, time:%i\n",
                (int)(*it)->getStartTime(), (int)(*it)->getEndTime(),
                (int)time(NULL));
#endif
            if (((*it)->getStartTime() <= time(NULL)) &&
                ((*it)->getEndTime() >= time(NULL)))
            {
                fprintf(stderr, "Starting to record\n");
                // start recording
                (*it)->start();
            }
        }
        else
        {
            // check to see any recordings that have to be stopped
            if ((*it)->getEndTime() <= time(NULL))
            {
                fprintf(stderr, "Stopping recording\n");
                (*it)->stop();
            }
        }
    }

    if (lastReap + REAP_INTERVAL <= time(NULL))
    {
        fprintf(stderr, "Reaping recordings\n");
        reapRecorders();
        lastReap = time(NULL) + REAP_INTERVAL;
    }
}

void StreamRecorder::getChannels(vector<Channel> &channels)
{
    ScopedLock lock(&mutex);
    xmltv.readChannels(channels);
}

void StreamRecorder::getProgrammes(const string &channelId,
    vector<Programme> &programmes, const string &date)
{
    ScopedLock lock(&mutex);
    xmltv.readProgrammes(channelId, programmes, date);
}

void StreamRecorder::getProgramme(const string &channelId,
    const string &start, Programme &programme)
{
    ScopedLock lock(&mutex);
    xmltv.readProgramme(channelId, start, programme);
}

bool StreamRecorder::record(const string &channelId,
    const string &start)
{
    ScopedLock lock(&mutex);
    Programme p;

    ChannelStream cs;
    bool found = false;
    // TOOD: return error if no channelstream is found
    for (unsigned int i = 0; i < settings.channelStreams.size(); i++)
    {
        if (settings.channelStreams[i].channel == channelId)
        {
            cs = settings.channelStreams[i];
            found = true;
        }
    }

    if (found)
    {
        xmltv.readProgramme(channelId, start, p);
        Recorder *r = new Recorder(cs.host, cs.port);
        r->setTitle(p.title[0].second);
        r->setDescription(p.description[0].second);

        struct tm t;
        strptime(start.substr(0, 13).c_str(), "%Y%m%d%H%M%S", &t);
        r->setStartTime(mktime(&t));
        strptime(p.end.substr(0, 13).c_str(), "%Y%m%d%H%M%S", &t);
        r->setEndTime(mktime(&t));

        // TODO: user should be able to set the filename
        r->setFilename(channelId + start + ".x264");

        recorders.push_back(r);
        fprintf(stderr, "Adding recorder\n");
    }

    return found;
}

void StreamRecorder::saveChannelStream(const string &channel,
    const string &host, const string &port)
{
    ScopedLock lock(&mutex);
    ChannelStream stream;
    stream.channel = channel;
    stream.host = host;
    stream.port = atoi(port.c_str());
    settings.channelStreams.push_back(stream);
    settings.save();
    // reload configuration with the new entry
    settings.reload();
}

void StreamRecorder::getRecordings(vector<RecorderInfo> &recordings)
{
    ScopedLock lock(&mutex);
    list<Recorder*>::iterator it;

    for (it = recorders.begin(); it != recorders.end(); it ++)
    {
        RecorderInfo info;
        info.startTime = (*it)->getStartTimeStr();
        info.endTime = (*it)->getEndTimeStr();
        recordings.push_back(info);
    }
}

void StreamRecorder::deleteChannelStream(unsigned int channelId)
{
    ScopedLock lock(&mutex);
    if ((channelId >= 0) && (channelId < settings.channelStreams.size()))
    {
        settings.channelStreams.erase(settings.channelStreams.begin() + channelId);
        settings.save();
        // Reload config file after deleting the entry
        settings.reload();
    }
}

void StreamRecorder::getChannelStreams(vector<ChannelStream> &streams)
{
    for (unsigned int i = 0; i < settings.channelStreams.size(); i++)
        streams.push_back(settings.channelStreams[i]);
}

void StreamRecorder::reapRecorders()
{
    list<Recorder*>::iterator tmp[MAX_RECORDERS];
    int num = 0;
    list<Recorder*>::iterator it;

    for (it = recorders.begin(); it != recorders.end(); it ++)
    {
        if (((*it)->getEndTime() <= time(NULL)) && (!(*it)->isRunning()))

        if (!(*it)->isRunning())
        {
            tmp[num++] = it;
            delete *it;
        }
    }

    for (int i = 0; i < num; i++)
        recorders.erase(tmp[i]);
}
