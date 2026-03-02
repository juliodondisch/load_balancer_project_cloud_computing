/// @file webserver.h
/// @brief declares the webserver class that processes one request at a time

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <string>
#include "request_generator.h"

/// @brief simulates a single web server that processes one request at a time
class WebServer {
public:
    /// @brief constructs a web server with the given ip address
    WebServer(string ip);

    /// @brief assigns a request to this server and marks it as busy
    void assignJob(Request r);

    /// @brief decrements time remaining by one cycle and frees the server when done
    void tick();

    /// @brief returns true if this server has no active job
    bool isFree() const;

    /// @brief returns the server ip address
    string getIP() const;

    /// @brief returns the request currently being processed
    Request getCurrentRequest() const;

private:
    string ip;           ///< ip address of this server
    bool busy;           ///< true if server is processing a request
    int time_remaining;  ///< clock cycles left for the current request
    Request current_request; ///< the request currently being processed
};

#endif
