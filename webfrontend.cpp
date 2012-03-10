/*
 * Copyright (C) 2012 Lars Hall
 *
 * This file is part of StreamRecorder.
 *
 * StreamRecorder is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * StreamRecorder is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with StreamRecorder.  If not, see <http://www.gnu.org/licenses/>.
 */

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

    if (request.path == "/get-channels")
        getChannels(contentType, output, request);
    else if (request.path == "/get-programmes")
        getProgrammes(contentType, output, request);
    else if (request.path == "/get-programme")
        getProgramme(contentType, output, request);
    else if (request.path == "/record")
        record(contentType, output, request);
    else if (request.path == "/save-channelstream")
        saveChannelStream(contentType, output, request);
    else if (request.path == "/get-channelstreams")
        getChannelStreams(contentType, output, request);
    else if (request.path == "/delete-channelstream")
        deleteChannelStream(contentType, output, request);
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
    vector<ChannelStream> channelStreams;
    stringstream ss;
    streamRecorder->getChannelStreams(channelStreams);
    ss << "{\"Channels\" : [";
    for (unsigned int i = 0; i < channelStreams.size(); i++)
    {
        ss << " { ";
        ss << "\"id\" : \"" + channelStreams[i].channel + "\" , ";
        ss << "\"displayname\" : \"" + channelStreams[i].channel + "\"";
        ss << " } ";

        if (i != channelStreams.size() - 1)
            ss << ",";
    }
    ss << "]}";

    output = ss.str();
}

void WebFrontend::getProgrammes(string &contentType, string &output,
    Request &request)
{
    HttpServer::RequestParams::const_iterator it;
    contentType = "application/json";
    string channelId = getParam(request, "channelid");
    string date = getParam(request, "date");

    vector<Programme> programmes;
    stringstream ss;

    streamRecorder->getProgrammes(channelId, programmes, date);
    ss << "{\"Programmes\" : [";
    for (unsigned int i = 0; i < programmes.size(); i++)
    {
        ss << " { ";
        ss << "\"start\" : \"" << programmes[i].start << "\" , ";
        ss << "\"end\" : \"" << programmes[i].end << "\" , ";
        ss << "\"title\" : \"" << programmes[i].title[0].second << "\"";
        ss << " } ";
        if (i != programmes.size() - 1)
            ss << ",";
    }
    ss << "]}";

    output = ss.str();
}

void WebFrontend::getProgramme(string &contentType, string &output,
    Request &request)
{
    HttpServer::RequestParams::const_iterator it;
    contentType = "application/json";
    string channelId = getParam(request, "channelid");

    vector<string> tokens;
    string start = getParam(request, "start");

    Programme programme;
    stringstream ss;
    streamRecorder->getProgramme(channelId, start, programme);
    ss << " { ";
    ss << "\"start\" : \"" << programme.start << "\" , ";
    if ((programme.title.size() > 0) && (programme.description.size() > 0))
    {
        ss << "\"title\" : \"" << programme.title[0].second << "\", ";
        ss << "\"start\" : \"" << programme.start << "\", ";
        ss << "\"description\" : \"" <<
            jsonEncode(programme.description[0].second) + "\"";
    }
    ss << " } ";

    output = ss.str();
}

void WebFrontend::record(string &contentType, string &output,
    Request &request)
{
    HttpServer::RequestParams::const_iterator it;
    contentType = "application/json";
    string channelId = getParam(request, "channelid");
    string start = getParam(request, "start");
    if (!streamRecorder->record(channelId, start))
    {
        // TODO: return error
    }
}

void WebFrontend::saveChannelStream(string &contentType, string &output,
    Request &request)
{
    if (request.type == HttpServer::HTTP_POST)
    {
        // settings page
        HttpServer::RequestParams::iterator it;
        string host = "";
        string port = "";
        string channel = "";
        for (it = request.params.begin(); it != request.params.end(); it++)
        {
            if (it->first == "channel")
                channel = it->second;
            else if (it->first == "host")
                host = it->second;
            else if (it->first == "port")
                port = it->second;
        }

        streamRecorder->saveChannelStream(channel, host, port);
    }
}

void WebFrontend::getChannelStreams(string &contentType, string &output,
    Request &request)
{
    contentType = "application/json";
    stringstream ss;
    vector<ChannelStream> channels;
    streamRecorder->getChannelStreams(channels);

    ss << "{\"ChannelStreams\" : [";
    for (unsigned int i = 0; i < channels.size(); i++)
    {
        ss << " { ";
        ss << "\"channelid\" : \"" << i << "\" , ";
        ss << "\"channel\" : \"" << channels[i].channel << "\" , ";
        ss << "\"host\" : \"" << channels[i].host << "\" , ";
        ss << "\"port\" : \"" << channels[i].port << "\"";
        ss << " } ";

        if (i != channels.size() - 1)
            ss << ",";
    }
    ss << "]}";
    output = ss.str();
}

void WebFrontend::deleteChannelStream(string &contentType, string &output,
    Request &request)
{
    vector<ChannelStream> channels;
    streamRecorder->getChannelStreams(channels);

    string channel = getParam(request, "channelid");
    if (channel != "")
    {
        int channelId = atoi(channel.c_str());
        if (channelId >= 0)
            streamRecorder->deleteChannelStream(channelId);

    }
}

string WebFrontend::getParam(const Request &request, const string &name)
{
    string ret = "";
    HttpServer::RequestParams::const_iterator it;
    for (it = request.params.begin(); it != request.params.end(); it++)
    {
        if (name == it->first)
        {
            ret = it->second;
            break;
        }
    }

    return ret;
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
            case '\r':
            case '\f':
            case '\b':
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
