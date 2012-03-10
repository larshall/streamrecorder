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

#include "settings.h"

Settings::Settings()
{
    filename = "";
}

bool Settings::load(const string &filename)
{
    this->filename = filename;
    pugi::xml_parse_result res = doc.load_file(filename.c_str());
    if (!res)
    {
        fprintf(stderr, "Cannot load xmlfile:%s", filename.c_str());
        return false;
    }
    
    readSettings();
    return true;

}

bool Settings::reload()
{
    if (channelStreams.size() != 0)
        channelStreams.clear();

    return load(filename);
}


bool Settings::readSettings()
{
    pugi::xml_node channelsNode = doc.child("settings").child("channelstreams");

    for (pugi::xml_node channelNode = channelsNode.child("channel");
        channelNode; channelNode = channelNode.next_sibling("channel"))
    {
        ChannelStream stream;
        stream.channel = channelNode.attribute("name").value();
        stream.host = channelNode.child("host").child_value();
        stream.port = atoi(channelNode.child("port").child_value());
        channelStreams.push_back(stream);
    }
    // TODO
    return true;
}

bool Settings::save()
{
    stringstream xml;
    xml << "<settings>\n";
    xml << "    <channelstreams>\n";
    for (unsigned int i = 0; i < channelStreams.size(); i++)
    {
        xml << "        <channel name=\"" <<
                        channelStreams[i].channel << "\">\n";
        xml << "            <host>" << channelStreams[i].host << "</host>\n";
        xml << "            <port>" << channelStreams[i].port << "</port>\n";
        xml << "        </channel>\n";
    }
    xml << "    </channelstreams>\n";
    xml << "</settings>";
    fprintf(stderr, "Saving settings:%s\n", xml.str().c_str());

    pugi::xml_document tmp;
    pugi::xml_parse_result res = tmp.load(xml.str().c_str());
    if (!res)
    {
        fprintf(stderr, "Cannot save xml document:%s\n", filename.c_str());
        return false;
    }

    return tmp.save_file(filename.c_str());
}
