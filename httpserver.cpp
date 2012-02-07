#include "httpserver.h"
#include "webfrontend.h"
/*
 * A simple blocking httpserver
 * Uhh keep it safe behind a firewall or something
 */
HttpServer::HttpServer(uint16_t port, const string &serverPath)
{
    this->port = port;
    started = false;
    this->serverPath = serverPath;
    this->frontend = new WebFrontend();
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
    {
        fprintf(stderr, "HttpServer: Error setting SO_REUSEADDR");
    }

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
        int newsockfd = accept(sockfd, (struct sockaddr *) &cliAddr,  &clientLen);
        memset(buffer, 0, HTTP_MAX_BUFFER);

        if (newsockfd)
        {
            int numbytes = read(newsockfd, buffer, HTTP_MAX_BUFFER);
            if (numbytes > 0)
            {
                string path = "";
                HttpServer::RequestType type = parseRequest(buffer, path);
                string html = "";

                // FIXME: not pretty. use bytes no matter what
                if (!frontend->handleRequest(html, type, path))
                {
                    // try to see there is a file in serverpath to send
                    uint8_t buffer[HTTP_MAX_FILE_SIZE];
                    memset(buffer, 0, HTTP_MAX_FILE_SIZE);
                    int size = loadFile(buffer, path);
                    send(newsockfd, buffer, size, 0);

                }
                else
                    send(newsockfd, html.c_str(), html.length(), 0);
                close(newsockfd);
            }
        }
    }
}

HttpServer::RequestType HttpServer::parseRequest(
    const string &req, string &path)
{
    char tmp[HTTP_MAX_TOKEN];
    string tokens[2];
    int numTokens = 0;
    RequestType ret = HTTP_UNKNOWN;

    strncpy(tmp, req.c_str(), HTTP_MAX_TOKEN);
    char *token = strtok (tmp, " ");
    path = "";

    while (token != NULL)
    {
        tokens[numTokens ++] = token;
        // Only request type and path are relevant
        if (numTokens >= 2)
            break;

        token = strtok (NULL, " ");
    }

    if (numTokens == 2)
    {
        if (tokens[0] == "GET")
            ret = HTTP_GET;
        else if (tokens[0] == "POST")
            ret = HTTP_POST;
        else if (tokens[0] == "PUT")
            ret = HTTP_PUT;
        else if (tokens[0] == "DELETE")
            ret = HTTP_DELETE;


        fprintf(stderr, "RequestType:%s\n", tokens[0].c_str());
        fprintf(stderr, "Path:%s\n", tokens[1].c_str());
        path = tokens[1];

    }

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


HttpServer::~HttpServer()
{
    delete frontend;
}
