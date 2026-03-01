/// @file webserver.cpp
/// @brief implements the webserver class

#include "webserver.h"

WebServer::WebServer(std::string ip)
    : ip(ip), busy(false), time_remaining(0) {
    current_request = {"", "", 0, ' '};
}

void WebServer::assignJob(Request r) {
    current_request = r;
    time_remaining = r.time;
    busy = true;
}

void WebServer::tick() {
    if (busy) {
        time_remaining--;
        if (time_remaining <= 0) {
            busy = false;
        }
    }
}

bool WebServer::isFree() const {
    return !busy;
}

std::string WebServer::getIP() const {
    return ip;
}

Request WebServer::getCurrentRequest() const {
    return current_request;
}
