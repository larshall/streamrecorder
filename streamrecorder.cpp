#include "rtp.h"
#include "h264.h"
#include "streamrecorder.h"
#include "recorder.h"

void saveFile()
{

}

int main()
{
    Recorder recorder("239.255.0.4", 1234);
    while(true)
    {
        recorder.run();
    }

    /*
    Rtp rtp;
    H264 h264;

    rtp.open("239.255.0.4");
    while(true)
    {
        RtpPacket *packet = rtp.readPacket();
        if (packet != NULL)
        {
            printf("\n---");

            printf("\nseqnum:%i", packet->seqnum);
            printf("\ntype:%i", packet->type);
            h264.parse(packet);
        }
    }
    */
}
