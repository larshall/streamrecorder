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

#include "xmltv.h"

bool XmlTv::loadFile(const string &filename, const string &lang)
{
    this->lang = lang;
    pugi::xml_parse_result res = doc.load_file(filename.c_str());
    if (!res)
    {
        fprintf(stderr, "Error: Cannot loadfile: %s\n", filename.c_str());
        return false;
    }

    return true;
}

void XmlTv::readChannels(vector<Channel> &channels)
{
    pugi::xml_node channelsNode = doc.child("tv");
    for (pugi::xml_node channelNode = channelsNode.child("channel");
        channelNode; channelNode = channelNode.next_sibling("channel"))
    {
          Channel c;
          c.id = channelNode.attribute("id").value();
          c.displayName = channelNode.child("display-name").child_value();
          channels.push_back(c);
    }
}

void XmlTv::readProgrammes(const string &channelId,
    vector<Programme> &programmes, const string &date)
{
    // End for each programme is start of next programme for specific channel
    // It is possible that end isn't defined (means xmltv file isn't updated)
    // this is an error and it'll be written to stderr for now
    int idx = 0;
    // Note: It doesn't care about timezones define in 'start' node
    string query  = "/tv/programme[@channel = '" + channelId +
        "' and contains(@start, " + date  + ")]";

    pugi::xpath_node_set progs = doc.select_nodes(query.c_str());
    for (pugi::xpath_node_set::const_iterator it = progs.begin();
        it != progs.end(); ++it)
    {
        pugi::xpath_node node = *it;
        string start = node.node().attribute("start").value();
        string title = node.node().child("title").child_value();
        Programme p;
        p.start = start;

        // updates the last entry with endtime
        if (idx != 0)
            programmes[idx-1].end = start;

        p.title.push_back(make_pair("da", title));
        programmes.push_back(p);
        idx ++;
    }
}

void XmlTv::readProgramme(const string &channelId,
    const string &start, Programme &programme)
{
    // NOTE: cannot query for 'start' attribute
    // because the endtime is the next programme's (in time) starttime
    string query  = "/tv/programme[@channel = '" + channelId + "']";
    bool found = false;

    pugi::xpath_node_set progs = doc.select_nodes(query.c_str());
    for (pugi::xpath_node_set::const_iterator it = progs.begin();
        it != progs.end(); ++it)
    {
        pugi::xpath_node node = *it;

        string pstart = node.node().attribute("start").value();
        if ((pstart.find(start) != string::npos) || (found))
        {
            if (found)
            {
                // updates the found programmes endtime
                // with the next programme's starttime
                programme.end = node.node().attribute("start").value();
                break;
            }

            string start = node.node().attribute("start").value();
            string title = node.node().child("title").child_value();
            string description = node.node().child("desc").child_value();
       
            programme.start = start;
            programme.title.push_back(make_pair("da", title));
            programme.description.push_back(make_pair("da", description));
            found = true;
        }
    }
}
