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
    bool found = true;
    string output = "";

    // gets all channels
    if (request.path == "/get-channels")
        getChannels(contentType, output, request);
    // gets a list of programmes for a specific channel
    else if (request.path == "/get-programmes")
        getProgrammes(contentType, output, request);
    // gets a specfic programme for a specific channel and time
    else if (request.path == "/get-programme")
        getProgramme(contentType, output, request);
    // records the requested programme
    else if (request.path == "/record")
        getProgramme(contentType, output, request);
    else
        found = false;

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
    if ((pos > 0) && (pos < (int)start.length()))
        start.insert(pos, " ");

    Programme programme;
    stringstream ss;
    streamRecorder->getProgramme(channelId, start, programme);
    ss << " { ";
    ss << "\"start\" : \"" + programme.start + "\" , ";
    if ((programme.title.size() > 0) && (programme.description.size() > 0))
    {
        ss << "\"title\" : \"" + programme.title[0].second + "\", ";
        ss << "\"description\" : \"" +
            jsonEncode(programme.description[0].second) + "\"";
    }
    ss << " } ";

    output = ss.str();
}

void WebFrontend::record(string &contentType, string &output,
    Request &request)
{

}

string WebFrontend::jsonEncode(const string &str)
{
    stringstream ss;
    for (unsigned int i = 0; i < str.length(); i++)
    {
        switch(str[i])
        {
            case '"':
                ss << "\\\"";
                break;
            case '\\':
                ss << "\\\\";
                break;
            case '/':
                break;
            case '\n':
                ss << " ";
                break;
            case '\r':
                ss << " ";
                break;
            case '\f':
                ss << " ";
                break;
            case '\b':
                ss << " ";
                break;
            case '\t':
                ss << " ";
                break;
            default:
                ss << str[i];
                break;
        }
    } 

    return ss.str();
}
