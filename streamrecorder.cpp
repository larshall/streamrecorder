#include "rtp.h"
#include "h264.h"
#include "streamrecorder.h"
#include "webfrontend.h"

int main()
{
    /*
    XmlTv xml;
    if (xml.loadFile("data/tv.xml"))
    {
        fprintf(stderr, "Loaded file\n");
        xml.readChannels();
    }
    */
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
/*
    Recorder recorder("239.255.0.4", 1234);
    recorder.start();
    while(true)
    {
        sleep(5);
        recorder.stop();
        sleep(1);
        break;
    }
*/
}

StreamRecorder::StreamRecorder()
{
    xmltv.loadFile("data/tv.xml", "da");
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
    r->setStartTime(time(NULL) + 10);
    r->setEndTime(time(NULL) + 20);
    // TODO: allow setting the filename
    r->setFilename(start + channelId + ".x264");

    recorders.push_back(r);
    fprintf(stderr, "Adding recorder\n");
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
