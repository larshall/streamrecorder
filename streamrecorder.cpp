#include "rtp.h"
#include "h264.h"
#include "streamrecorder.h"
#include "recorder.h"

int main()
{
    HttpServer server(8182, "./data");
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
}
