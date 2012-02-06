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
#include "thread.h"

#define HTTP_MAX_PENDING 10
#define HTTP_MAX_BUFFER 1024

using std::string;

class HttpServer : public Thread
{
    private:
        bool started;
        int sockfd;
        short port;
        void startServer();
        string processRequest();
        void parseRequest(const string &req);

    public:
        HttpServer(uint16_t port);
        ~HttpServer();
        void run();
};

#endif
