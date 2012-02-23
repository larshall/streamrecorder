#include "rtp.h"
#include "h264.h"
#include "streamrecorder.h"
#include "webfrontend.h"

int main()
{
    StreamRecorder streamRecorder;
    WebFrontend frontend(&streamRecorder);

    HttpServer server(8182, "./data", &frontend);
    server.start();
    while(true)
    {
        streamRecorder.process();
        // once a second is enough for recordings
        sleep(1);
    }
}

StreamRecorder::StreamRecorder()
{
    xmltv.loadFile("data/tv.xml", "da");
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
            fprintf(stderr, "startTime:%i, time:%i\n",
                (int)(*it)->getStartTime(), (int)time(NULL));
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
    vector<Programme> &programmes)
{
    ScopedLock lock(&mutex);
    xmltv.readProgrammes(channelId, programmes);
}

void StreamRecorder::getProgramme(const string &channelId,
    const string &start, Programme &programme)
{
    ScopedLock lock(&mutex);
    xmltv.readProgramme(channelId, start, programme);
}

void StreamRecorder::record(const string &channelId, const string &start)
{
    ScopedLock lock(&mutex);
    Programme p;
    xmltv.readProgramme(channelId, start, p);

    Recorder *r = new Recorder("239.255.0.4", 1234);
    r->setTitle(p.title[0].second);
    r->setDescription(p.description[0].second);
    r->setStartTime(time(NULL) + 1);
    r->setEndTime(time(NULL) + 120);
    // TODO: user should be able to set the filename
    r->setFilename(channelId + start + ".x264");

    recorders.push_back(r);
    fprintf(stderr, "Adding recorder\n");
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

void StreamRecorder::deleteChannelStream(const string &channel)
{
    ScopedLock lock(&mutex);
    int idx = -1;
    for (unsigned int i = 0; i < settings.channelStreams.size(); i++)
    {
        if (settings.channelStreams[i].channel == channel)
            idx = i;
    }
    if ((idx >= 0) && (idx < settings.channelStreams.size()))
        settings.channelStreams.erase(settings.channelStreams.begin() + idx);
    settings.save();
    // reload config file after deleting the entry
    settings.reload();
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
        if (!(*it)->isRunning())
        {
            tmp[num++] = it;
            delete *it;
        }
    }

    for (int i = 0; i < num; i++)
        recorders.erase(tmp[i]);
}
