#include "httpserver.h"

/*
 * A simple blocking httpserver
 * Uhh keep it safe behind a firewall or something
 */
HttpServer::HttpServer(uint16_t port)
{
    this->port = port;
    started = false;
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
                parseRequest(buffer);
                string html = processRequest();
                send(newsockfd, html.c_str(), html.length(), 0);
                close(newsockfd);
            }
        }
    }
}

string HttpServer::processRequest()
{
    string ret = "HTTP/1.0 0 OK\r\n\r\n";
    ret += "<html><body><b>HEJ</b></body></html>\r\n";

    return ret;
}

void HttpServer::parseRequest(const string &req)
{

}

HttpServer::~HttpServer()
{

}
