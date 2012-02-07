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
#include "thread.h"

#define HTTP_MAX_PENDING 10
#define HTTP_MAX_BUFFER 1024
#define HTTP_MAX_TOKEN 1024
#define HTTP_MAX_FILE_SIZE 100000

using std::string;
using std::ifstream;
using std::ios;

class WebFrontend;


class HttpServer : public Thread
{
    public:
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
        bool started;
        int sockfd;
        short port;
        void startServer();
        RequestType parseRequest(const string &req, string &path);
        string serverPath;
        // returns -1 if file not found or cannot read
        int loadFile(uint8_t *bytes, const string &filename);

    public:
        HttpServer(uint16_t port, const string &serverPath);
        ~HttpServer();
        void run();
};

#endif
