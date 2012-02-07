#ifndef WEBFRONTEND_H
#define WEBFRONTEND_H

#include <string>
#include "httpserver.h"

using std::string;

class WebFrontend
{
    public:
        WebFrontend();
        // returns true if page is found
        bool handleRequest(string &html,
            HttpServer::RequestType type, const string &req);
};

#endif
