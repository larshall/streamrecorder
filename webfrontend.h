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

        /// Get all channels from a xmltv file
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
        /// Saves channel stream
        void saveChannelStream(string &contentType, string &output,
            Request &request);
        /// Get all channel streams configured
        void getChannelStreams(string &contentType, string &output,
            Request &request);
        /// Deletes the specified channelstream (id as param and it
        /// is just the index in the vector
        void deleteChannelStream(string &contentType, string &output,
            Request &request);

        void getRecordings(string &contentType, string &output,
            Request &request);

        string getParam(const Request &request, const string &name);
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
