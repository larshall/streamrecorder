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
