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
    vector<Programme> &programmes)
{
    string query  = "/tv/programme[@channel = '" + channelId + "']";
    pugi::xpath_node_set tools = doc.select_nodes(query.c_str());
    for (pugi::xpath_node_set::const_iterator it = tools.begin();
        it != tools.end(); ++it)
    {
        pugi::xpath_node node = *it;
        string start = node.node().attribute("start").value();
        string title = node.node().child("title").child_value();
        Programme p;
        p.start = start;
        p.title.push_back(make_pair("da", title));
        programmes.push_back(p);
    }
}

void XmlTv::readProgramme(const string &channelId,
    const string &start, Programme &programme)
{
    string query  = "/tv/programme[@channel = '" + channelId +
        "' and @start = '" + start + "']";

    std::cout << "\nQuery:" << query << std::endl;
    pugi::xpath_node_set tools = doc.select_nodes(query.c_str());
    for (pugi::xpath_node_set::const_iterator it = tools.begin();
        it != tools.end(); ++it)
    {
        pugi::xpath_node node = *it;
        string start = node.node().attribute("start").value();
        string title = node.node().child("title").child_value();
        string description = node.node().child("desc").child_value();

        programme.start = start;
        programme.title.push_back(make_pair("da", title));
        programme.description.push_back(make_pair("da", description));
    }
}
