#include "rtp.h"
#include "h264.h"
#include "streamrecorder.h"
#include "recorder.h"
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
    numRecordings = 0;
    xmltv.loadFile("data/tv.xml", "da");
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
