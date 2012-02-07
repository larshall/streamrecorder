#include "webfrontend.h"

WebFrontend::WebFrontend()
{}

bool WebFrontend::handleRequest(string &html,
    HttpServer::RequestType type, const string &req)
{
    bool ret = false;

    if (req == "/")
    {
        html = "HTTP/1.0 0 OK\r\n\r\n";
        html += "<html><body>streamrecorder</body></html>\r\n";
        ret = true;
       
    }
    if (req == "/test")
    {
        html = "HTTP/1.0 0 OK\r\n\r\n";
        html += "<html><body><b>TEST</b><img src=\"./logo.png\" /></body></html>\r\n";
        ret = true;
    }
    else if (req == "/getchannels")
    {
        
    }

    return ret;
}
