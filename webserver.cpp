/// @file webserver.cpp
/// @brief implements the webserver class

#include "webserver.h"

using namespace std;

/// @brief constructs a web server with the given ip and initializes it to idle
WebServer::WebServer(string ip)
    : ip(ip), busy(false), time_remaining(0) {
    current_request = {"", "", 0, ' '};
}

/// @brief stores the request and marks the server as busy
void WebServer::assignJob(Request r) {
    current_request = r;
    time_remaining = r.time;
    busy = true;
}

/// @brief decrements the remaining time by one cycle and idles the server when done
void WebServer::tick() {
    if (busy) {
        time_remaining--;
        if (time_remaining <= 0) {
            busy = false;
        }
    }
}

/// @brief returns true if the server has no active job
bool WebServer::isFree() const {
    return !busy;
}

/// @brief returns the ip address of this server
string WebServer::getIP() const {
    return ip;
}

/// @brief returns the request currently assigned to this server
Request WebServer::getCurrentRequest() const {
    return current_request;
}
