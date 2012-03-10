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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "pugixml/pugixml.hpp"

using std::string;
using std::vector;
using std::stringstream;

struct ChannelStream
{
    string channel;
    string host;
    uint16_t port;
};

class Settings
{
    private:
        pugi::xml_document doc;
        bool readSettings();
        string filename;

    public:
        vector<ChannelStream> channelStreams;

        Settings();
        bool load(const string &filename);
        bool reload();
        bool save();

};

#endif
