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

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <fstream>
#include <sys/stat.h>
#include <list>
#include <vector>
#include <iostream>
#include "thread.h"

#define HTTP_MAX_PENDING 10
#define HTTP_MAX_BUFFER 1024
#define HTTP_MAX_TOKEN 1024
#define HTTP_MAX_FILE_SIZE 100000

using std::string;
using std::ifstream;
using std::ios;
using std::pair;
using std::list;
using std::vector;

class WebFrontend;
struct Request;

class HttpServer : public Thread
{
    public:
        typedef list<pair<string, string> > RequestParams;

        enum RequestType
        {
            HTTP_GET = 0,
            HTTP_POST,
            HTTP_DELETE,
            HTTP_PUT,
            HTTP_UNKNOWN
        };

    private:
        WebFrontend *frontend;
        string serverPath;
        bool started;
        int sockfd;
        short port;

        void startServer();
        void parseRequest(Request &request, const string &req);

        /// Retuns start of param list if any otherwise -1
        int parseReqParams(RequestParams &params,
            const string &req, bool isPost);
        /// returns -1 if file not found or cannot read
        int loadFile(uint8_t *bytes, const string &filename);
        string createHeader(int status, const string &contentType,
            const Request &request, const string &reason = "");


        /// Url decodes a string (percent decoding)
        string percentDecode(const string &data);
    public:
        HttpServer(uint16_t port, const string &serverPath,
            WebFrontend *frontend);
        static void splitString(vector<string> &tokens,
            const string &str, const string &sep);
        ~HttpServer();
        void run();
};

struct Request
{
    string requestStr;
    string path;
    HttpServer::RequestType type;
    HttpServer::RequestParams params;
};



#endif
