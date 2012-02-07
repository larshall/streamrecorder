#ifndef WEBFRONTEND_H
#define WEBFRONTEND_H

#include <string>

using std::string;

class WebFrontend
{
    public:
        WebFrontend();
        bool handleRequest(const string &req);
};

#endif
