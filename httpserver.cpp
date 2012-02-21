#include "httpserver.h"
#include "webfrontend.h"
/*
 * A simple blocking httpserver
 * Uhh keep it safe behind a firewall or something
 *
 * TODO:
 * Add more status codes
 * Add charset
 * Add content type
 */
HttpServer::HttpServer(uint16_t port, const string &serverPath,
    WebFrontend *frontend)
{
    this->port = port;
    started = false;
    this->serverPath = serverPath;
    this->frontend = frontend;
}

void HttpServer::startServer()
{
    struct sockaddr_in name;

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        fprintf(stderr, "HttpServer: Error creating socket");

    int reuse = 1;
    
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
        (char *)&reuse, sizeof(reuse)) < 0)
        fprintf(stderr, "HttpServer: Error setting SO_REUSEADDR");

    name.sin_family = AF_INET;
    name.sin_port = htons (port);
    name.sin_addr.s_addr = htonl (INADDR_ANY);

    if (bind (sockfd, (struct sockaddr *) &name, sizeof (name)) < 0)
        fprintf(stderr, "HttpServer: Error binding socket\n");
    if (listen (sockfd, HTTP_MAX_PENDING) < 0)
        fprintf(stderr, "HttpServer: Error listening\n");
}

void HttpServer::run()
{
    char buffer[HTTP_MAX_BUFFER];

    if (!started)
        startServer();

    while(true)
    {
        struct sockaddr_in cliAddr;
        socklen_t clientLen = sizeof(cliAddr);
        int newsockfd = 
            accept(sockfd, (struct sockaddr *) &cliAddr,  &clientLen);
        memset(buffer, 0, HTTP_MAX_BUFFER);

        if (newsockfd)
        {
            int numbytes = read(newsockfd, buffer, HTTP_MAX_BUFFER);
            if (numbytes > 0)
            {
                string contentType = "";

                Request request;
                parseRequest(request, buffer);
                // TODO: log
                //printf("request:%s\n", request.requestStr.c_str());

                uint8_t bytes[HTTP_MAX_FILE_SIZE];
                memset(bytes, 0, HTTP_MAX_FILE_SIZE);

                int size = 0;
                if ((size = frontend->handleRequest(
                    contentType, bytes, request)) == -1)
                {
                    // try to see there is a file in serverpath to send
                    if (request.path == "/")
                        request.path = "/index.html";
                    size = loadFile(bytes, request.path);
                }
                
                char tmp[HTTP_MAX_FILE_SIZE];
                memset(tmp, 0, HTTP_MAX_FILE_SIZE);
                string header = ""; 

                // Not pretty. Only 200 and 404 are implemented
                if (size > -1)
                {
                    header = createHeader(200, contentType, request);
                    memcpy(tmp, header.c_str(), header.length());
                    memcpy(tmp + header.length(), bytes, size);
                }
                else
                {
                    size = 0;
                    header = createHeader(404, contentType,
                        request, "404 Not Found");
                    memcpy(tmp, header.c_str(), header.length());
                }

                send(newsockfd, tmp, header.length() + size, 0);

                close(newsockfd);
            }
        }
    }
}

void HttpServer::parseRequest(Request &request, const string &req)
{
    vector<string> tokens;
    splitString(tokens, req, " ");
    request.requestStr = req;

    if (tokens.size() >= 2)
    {
        if (tokens[0] == "GET")
            request.type = HTTP_GET;
        // Note: Only get request is supported
        else if (tokens[0] == "POST")
            request.type = HTTP_POST;
        else if (tokens[0] == "PUT")
            request.type = HTTP_PUT;
        else if (tokens[0] == "DELETE")
            request.type = HTTP_DELETE;

        string decoded = percentDecode(tokens[1].c_str());
        int pos = parseReqParams(request.params, decoded, false);

        std::cout << "\nREQUEST:" << req << std::endl;
        if (pos > -1)
            request.path = decoded.substr(0, (int)pos);
        else
            request.path = decoded;
    }

    if (request.type == HTTP_POST)
    {
        unsigned int bodyPos = req.find("\r\n\r\n");
        if ((bodyPos != string::npos) && (bodyPos < req.length() - 1))
        {
            // extract the body (after "\r\n\r\n")
            string body =
                percentDecode(req.substr(bodyPos + 3, req.length()));
            RequestParams post;
            parseReqParams(post, body, true);
            RequestParams::iterator it;
            // Hack: appending post params to request params
            // (treating post key/vals like get key/vals)
            for (it = post.begin(); it != post.end(); it++)
                request.params.push_back(make_pair(it->first, it->second));
        }
    }
}

int HttpServer::parseReqParams(RequestParams &params,
    const string &req, bool isPost)
{
    int ret = -1;
    int pos = 0;

    if (!isPost)
        pos = req.find('?');

    if (((pos != (int)string::npos) && ((int)req.length() - 1 > pos)) ||
        isPost)
    {
        ret = pos;
        // skipping '?'
        string str = req.substr(pos + 1, req.length()); 
        vector<string> keyvals;
        // split per '&'
        splitString(keyvals, str, "&");
        for (unsigned int i = 0; i < keyvals.size(); i++)
        {
            // splits key=val
            vector<string> tokens;
            splitString(tokens, keyvals[i], "=");
            if (tokens.size() > 0)
            {
                string key = "";
                string value = "";

                key = tokens[0];
                if (tokens.size() >= 2)
                    value = tokens[1];

                params.push_back(make_pair(key, value));
            }
        }
    }

    return ret;
}

string HttpServer::percentDecode(const string &data)
{
    string ret = "";
    unsigned int i = 0;
    while(i < data.size())
    {
        if ((data[i] == '%') && (i < data.size() - 2))
        {
            char str[2];
            str[0] = data[i+1];
            str[1] = data[i+2];
            uint64_t tmp = strtoul(str, NULL, 16);
            ret += (char)tmp;
            i += 2;
        }
        else if (data[i] == '+')
            ret += " ";
        else
            ret += data[i];

        i++;
    }

    return ret;
}

string HttpServer::createHeader(int status, const string &contentType,
    const Request &request, const string &reason)
{
    string ret = "";
    string version = "HTTP/1.0";
    string type = "text/plain";

    if (status == 200)
        ret  = version + " 200 OK";
    else if (status == 404)
        ret  = version + " 404 Not Found";

    if ((status == 200) && (contentType == ""))
    {
        // trying to guess the content type
        // by .extension
        // Uhh not pretty
        if (request.path.find(".png") != string::npos)
            type = "image/png";
        else if (request.path.find(".json") != string::npos)
            type = "application/json";
        else
            type = "text/html";
    }
    else
    {
        if (contentType != "")
            type = contentType;

        ret += "\r\nContent-Type:" + type + "\r\n\r\n";
    }

    ret += "\r\n\r\n";

    if (reason != "")
        ret += reason;

    return ret;
}


int HttpServer::loadFile(uint8_t *bytes, const string &filename)
{
    int ret = -1;
    ifstream file((serverPath + filename).c_str(),
        ios::in|ios::binary|ios::ate);

    if (file.is_open())
    {
        size_t size = file.tellg();
        file.seekg(0, ios::beg);
        file.read((char*)bytes, size);
        file.close();
        ret = true;
        ret = size;
    }

    return ret;
}

void HttpServer::splitString(vector<string> &tokens,
    const string &str, const string &sep)
{
    char tmp[HTTP_MAX_TOKEN];
    memset(tmp, 0, HTTP_MAX_TOKEN);
    strncpy(tmp, str.c_str(), HTTP_MAX_TOKEN);
    char *token = strtok (tmp, sep.c_str());

    while (token != NULL)
    {
        tokens.push_back(token);
        token = strtok (NULL, sep.c_str());
    }
}

HttpServer::~HttpServer()
{
}
