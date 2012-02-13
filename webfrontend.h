#ifndef WEBFRONTEND_H
#define WEBFRONTEND_H

#include <string>
#include <sstream>
#include "httpserver.h"

using std::string;
using std::stringstream;

class StreamRecorder;

class WebFrontend
{
    private:
        StreamRecorder *streamRecorder;

        void getChannels(string &contentType, string &output,
            Request &request);
        void getProgrammes(string &contentType, string &output,
            Request &request);
        void getProgramme(string &contentType, string &output,
            Request &request);
    public:
        WebFrontend(StreamRecorder *streamRecorder);
        // returns the size (>0) if a page is found
        int handleRequest(string &contentType, uint8_t *bytes,
            Request &requst);
};

#endif
