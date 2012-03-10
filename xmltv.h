#ifndef XMLTV_H
#define XMLTV_H

#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>
// PugiXML now supports xpaths with exception disabled
#include "pugixml/pugixml.hpp"

using std::string;
using std::vector;
using std::pair;
using std::make_pair;
using std::cout;
using std::endl;

struct Channel
{
    string id;
    string displayName;
};

struct Programme
{
    // TODO: convert to datetime
    string start;
    string end;
    string channeldId;
    // lang, text
    vector<pair<string, string> > title;
    vector<pair<string, string> > description;
    vector<pair<string, string> > categories;
};

/**
 * XmlTv reads a xmltv file.
 * TODO: buffer some of the information istead of
 * parsing the xml file every time
 */
class XmlTv
{
    private:
        pugi::xml_document doc;
        string lang;

    public:
        XmlTv(){};
        bool loadFile(const string &filename, const string &lang);
        void readChannels(vector<Channel> &channels);
        void readProgrammes(const string &channelId,
            vector<Programme> &programmes, const string &date);
        void readProgramme(const string &channelId,
            const string &start, Programme &programme);
};

#endif
