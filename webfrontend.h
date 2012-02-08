#ifndef WEBFRONTEND_H
#define WEBFRONTEND_H

#include <string>
#include "httpserver.h"

using std::string;

class WebFrontend
{
    public:
        WebFrontend();
        // returns the size (>0) if a page is found
        int handleRequest(string &contentType, uint8_t *bytes,
            HttpServer::RequestType type, const string &req);
};

#endif
