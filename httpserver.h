#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <stdint.h>
#include "thread.h"

class HttpServer : public Thread
{
    public:
        HttpServer(uint16_t port);
        ~HttpServer();

        void run();
};

#endif
