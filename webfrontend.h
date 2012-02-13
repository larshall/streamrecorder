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

        /// Get all channels
        void getChannels(string &contentType, string &output,
            Request &request);
        //// Get programmes for specified channel
        void getProgrammes(string &contentType, string &output,
            Request &request);
        /// Get programmes for channel at specified time
        void getProgramme(string &contentType, string &output,
            Request &request);
        /// Records specified channel at specfied time
        void record(string &contentType, string &output,
            Request &request);

        string jsonEncode(const string &str);
    public:
        WebFrontend(StreamRecorder *streamRecorder);
        /**
         * Handles a request from the HttpServer
         * @param[out] contentType The contentType of the found page
         * @param[out] bytes The bytes of the found page
         * @return The size (>0 if page is found)
         */
        int handleRequest(string &contentType, uint8_t *bytes,
            Request &requst);
};

#endif
