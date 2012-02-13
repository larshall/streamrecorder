#include "webfrontend.h"
#include "streamrecorder.h"

WebFrontend::WebFrontend(StreamRecorder *streamRecorder)
{
    this->streamRecorder = streamRecorder;
}

int WebFrontend::handleRequest(string &contentType, uint8_t *bytes,
    Request &request)
{
    int ret = -1;
    bool found = false;
    string output = "";

    // gets all channels
    if (request.path == "/get-channels")
    {
        getChannels(contentType, output, request);
        found = true;
    }
    // gets a list of programmes for a specific channel
    else if (request.path == "/get-programmes")
    {
        getProgrammes(contentType, output, request);
        found = true;
    }
    // gets a specfic programme for a specific channel and time
    else if (request.path == "/get-programme")
    {
        getProgramme(contentType, output, request);
        found = true;
    }

    if (found)
    {
        ret = output.length();
        memcpy(bytes, output.c_str(), output.length());
    }

    return ret;
}

void WebFrontend::getChannels(string &contentType, string &output,
    Request &request)
{
    contentType = "application/json";
    vector<Channel> channels;
    stringstream ss;
    streamRecorder->getChannels(channels);
    ss << "{\"Channels\" : [";
    for (unsigned int i = 0; i < channels.size(); i++)
    {
        ss << " { ";
        ss << "\"id\" : \"" + channels[i].id + "\" , ";
        ss << "\"displayname\" : \"" + channels[i].displayName + "\"";
        ss << " } ";
        // TODO: create a general vector->json array function
        if (i != channels.size() - 1)
            ss << ",";
    }
    ss << "]}";

    output = ss.str();
}

void WebFrontend::getProgrammes(string &contentType, string &output,
    Request &request)
{
    string channelId = "";
    HttpServer::RequestParams::const_iterator it;
    contentType = "application/json";

    for (it = request.params.begin(); it != request.params.end(); it ++)
    {
        if (it->first == "channelid")
            channelId = it->second;
    }

    vector<Programme> programmes;
    stringstream ss;
    streamRecorder->getProgrammes(channelId, programmes);
    ss << "{\"Programmes\" : [";
    for (unsigned int i = 0; i < programmes.size(); i++)
    {
        ss << " { ";
        ss << "\"start\" : \"" + programmes[i].start + "\" , ";
        ss << "\"title\" : \"" + programmes[i].title[0].second + "\"";
        ss << " } ";
        // TODO: create a general vector->json array function
        if (i != programmes.size() - 1)
            ss << ",";
    }
    ss << "]}";

    output = ss.str();
}

void WebFrontend::getProgramme(string &contentType, string &output,
    Request &request)
{
    string channelId = "";
    string start = "";
    HttpServer::RequestParams::const_iterator it;
    contentType = "application/json";

    for (it = request.params.begin(); it != request.params.end(); it ++)
    {
        if (it->first == "channelid")
            channelId = it->second;
        if (it->first == "start")
            start = it->second;
    }

    // Hack: the httpserver doesn't support urldecode yet,
    // so space in iso8601 extended date is removed
    int pos = start.find("+");
    start.insert(pos, " ");

    Programme programme;
    stringstream ss;
    streamRecorder->getProgramme(channelId, start, programme);
    ss << "{\"Programme\" : [";
    ss << " { ";
    ss << "\"start\" : \"" + programme.start + "\" , ";
    if ((programme.title.size() > 0) && (programme.description.size() > 0))
    {
        ss << "\"title\" : \"" + programme.title[0].second + "\"";
        ss << "\"description\" : \"" + programme.description[0].second + "\"";
    }
    ss << " } ";
    // TODO: create a general vector->json array function
    ss << "]}";

    output = ss.str();

}

