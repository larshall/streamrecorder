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
    //streamRecorder.getProgrammes("3plus");

    HttpServer server(8182, "./data", &frontend);
    server.start();
    while(true)
    {
        streamRecorder.process();
        usleep(0.1);
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
}

void StreamRecorder::process()
{
    reapRecorders();
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

    Recorder *r = new Recorder("239.255.0.1", 1234);
    r->setTitle(p.title[0].second);
    r->setDescription(p.description[0].second);
    r->setStartTime(time(NULL));
    r->setStartTime(time(NULL) + 60);

    recorders.push_back(r);
}

void StreamRecorder::reapRecorders()
{
    ScopedLock lock(&mutex);
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
    /*
    int tmp[MAX_RECORDERS];
    int num = 0;
    memset(tmp, 0, MAX_RECORDERS);
    */
    /*
    for (int i = 0; i < recorders.size(); i++)
    {
        if (recorders[i].running() == false)
        {
            tmp[num++] = i;
            delete recorders[i];
        }
    }

    for (int i = 0; i < num; i++)
    {
        recorders.erase
    }
    */
}
