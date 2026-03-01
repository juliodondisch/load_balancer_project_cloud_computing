/// @file load_balancer.cpp
/// @brief implements the loadbalancer class

#include "load_balancer.h"
#include <cstdlib>
#include <iostream>
#include <algorithm>

LoadBalancer::LoadBalancer(char type, int initial_servers, int cooldown_n)
    : job_type(type), clock(0), adjust_cooldown(0), cooldown_period(cooldown_n),
      total_processed(0), min_servers_seen(initial_servers), max_servers_seen(initial_servers) {

    std::string filename = std::string("lb_log_") + type + ".txt";
    log_file.open(filename);

    for (int i = 0; i < initial_servers; i++) {
        servers.push_back(new WebServer(randomIP()));
    }

    log("load balancer [" + std::string(1, job_type) + "] started with " +
        std::to_string(initial_servers) + " servers");
}

LoadBalancer::~LoadBalancer() {
    log_file << "\n--- summary ---\n";
    log_file << "total requests processed: " << total_processed << "\n";
    log_file << "min servers used: " << min_servers_seen << "\n";
    log_file << "max servers used: " << max_servers_seen << "\n";
    log_file << "final queue size: " << pending.size() << "\n";
    log_file << "total clock cycles run: " << clock << "\n";
    log_file.close();

    for (WebServer* s : servers) {
        delete s;
    }
}

void LoadBalancer::addRequest(Request r) {
    pending.push(r);
}

void LoadBalancer::tick() {
    clock++;
    assignJobs();
    tickServers();
    if (adjust_cooldown > 0) {
        adjust_cooldown--;
    } else {
        checkQueueBalance();
    }
}

void LoadBalancer::assignJobs() {
    for (WebServer* s : servers) {
        if (s->isFree() && !pending.empty()) {
            Request r = pending.front();
            pending.pop();
            s->assignJob(r);
            log("[clock " + std::to_string(clock) + "] assigned request from " +
                r.ip_in + " to server " + s->getIP() +
                " (queue: " + std::to_string(pending.size()) + " remaining)");
        }
    }
}

void LoadBalancer::tickServers() {
    for (WebServer* s : servers) {
        bool was_busy = !s->isFree();
        s->tick();
        if (was_busy && s->isFree()) {
            total_processed++;
            log("[clock " + std::to_string(clock) + "] server " + s->getIP() +
                " completed job (total processed: " + std::to_string(total_processed) + ")");
        }
    }
}

void LoadBalancer::checkQueueBalance() {
    int n = (int)servers.size();
    int q = (int)pending.size();

    if (q < 50 * n && n > 1) {
        removeServer();
        adjust_cooldown = cooldown_period;
        log("[clock " + std::to_string(clock) + "] queue low (size " + std::to_string(q) +
            "), removed server (servers: " + std::to_string(servers.size()) + ")");
    } else if (q > 80 * n) {
        addServer();
        adjust_cooldown = cooldown_period;
        log("[clock " + std::to_string(clock) + "] queue high (size " + std::to_string(q) +
            "), added server (servers: " + std::to_string(servers.size()) + ")");
    }
}

void LoadBalancer::addServer() {
    servers.push_back(new WebServer(randomIP()));
    max_servers_seen = std::max(max_servers_seen, (int)servers.size());
}

void LoadBalancer::removeServer() {
    for (int i = 0; i < (int)servers.size(); i++) {
        if (servers[i]->isFree()) {
            delete servers[i];
            servers.erase(servers.begin() + i);
            min_servers_seen = std::min(min_servers_seen, (int)servers.size());
            return;
        }
    }
    // all servers busy, skip removal this cycle
}

void LoadBalancer::log(const std::string& msg) {
    if (log_file.is_open()) {
        log_file << msg << "\n";
    }
}

std::string LoadBalancer::randomIP() {
    return std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256);
}

int LoadBalancer::serverCount() const {
    return (int)servers.size();
}

int LoadBalancer::queueSize() const {
    return (int)pending.size();
}

void LoadBalancer::printSummary() {
    std::cout << "--- load balancer [" << job_type << "] summary ---\n";
    std::cout << "total requests processed: " << total_processed << "\n";
    std::cout << "min servers used: " << min_servers_seen << "\n";
    std::cout << "max servers used: " << max_servers_seen << "\n";
    std::cout << "final queue size: " << pending.size() << "\n";
    std::cout << "clock cycles run: " << clock << "\n";
}
