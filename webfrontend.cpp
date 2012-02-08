#include "webfrontend.h"

WebFrontend::WebFrontend()
{}

int WebFrontend::handleRequest(string &contentType, uint8_t *bytes,
    HttpServer::RequestType type, const string &req)
{
    int ret = -1;
    bool found = false;
    string html = "";

    if (req == "/")
    {
        html += "<html><body>streamrecorder</body></html>";
        found = true;
       
    }
    if (req == "/test")
    {
        html += "<html><body><b>TEST</b><img src=\"./logo.png\" /></body></html>";
        found = true;
    }
    else if (req == "/get-channels")
    {
        contentType = "application/json";
        html += "{ \"id\" : \"DR1\" , \"type\" : \"FILM\" }";
        found = true;
    }

    if (found)
    {
        ret = html.length();
        memcpy(bytes, html.c_str(), html.length());
    }

    return ret;
}
