/// @file load_balancer.cpp
/// @brief implements the loadbalancer class

#include "load_balancer.h"
#include "colors.h"
#include <cstdlib>
#include <iostream>
#include <algorithm>

using namespace std;

/// @brief opens the log file and creates the initial server pool
LoadBalancer::LoadBalancer(char type, int initial_servers, int cooldown_n)
    : job_type(type), clock(0), adjust_cooldown(0), cooldown_period(cooldown_n),
      total_processed(0), min_servers_seen(initial_servers), max_servers_seen(initial_servers) {

    string filename = string("lb_log_") + type + ".txt";
    log_file.open(filename);

    for (int i = 0; i < initial_servers; i++) {
        servers.push_back(new WebServer(randomIP()));
    }

    log("load balancer [" + string(1, job_type) + "] started with " +
        to_string(initial_servers) + " servers");
}

/// @brief writes the summary to the log file and frees all server memory
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

/// @brief pushes a request onto the pending queue
void LoadBalancer::addRequest(Request r) {
    pending.push(r);
}

/// @brief advances one clock cycle by assigning jobs, ticking servers, and checking balance
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

/// @brief pairs each idle server with the next pending request
void LoadBalancer::assignJobs() {
    for (WebServer* s : servers) {
        if (s->isFree() && !pending.empty()) {
            Request r = pending.front();
            pending.pop();
            s->assignJob(r);
            log("[clock " + to_string(clock) + "] assigned request from " + r.ip_in + " to server " + s->getIP() + " (queue: " + to_string(pending.size()) + " remaining)");
        }
    }
}

/// @brief ticks every server and logs any that finish their current job
void LoadBalancer::tickServers() {
    for (WebServer* s : servers) {
        bool was_busy = !s->isFree();
        s->tick();
        if (was_busy && s->isFree()) {
            total_processed++;
            log("[clock " + to_string(clock) + "] server " + s->getIP() + " completed job (total processed: " + to_string(total_processed) + ")");
        }
    }
}

/// @brief adds or removes one server if queue depth is outside the target range
void LoadBalancer::checkQueueBalance() {
    int n = (int)servers.size();
    int q = (int)pending.size();

    if (q < 50 * n && n > 1) {
        removeServer();
        adjust_cooldown = cooldown_period;
        cout << YELLOW << "[LB-" << job_type << "] queue low (" << q << "), removed a server (now " << servers.size() << " servers)" << RESET << "\n";
        log("[clock " + to_string(clock) + "] queue low (size " + to_string(q) + "), removed server (servers: " + to_string(servers.size()) + ")");
    }
    else if (q > 80 * n) {
        addServer();
        adjust_cooldown = cooldown_period;
        cout << RED << "[LB-" << job_type << "] queue high (" << q << "), added a server (now " << servers.size() << " servers)" << RESET << "\n";
        log("[clock " + to_string(clock) + "] queue high (size " + to_string(q) + "), added server (servers: " + to_string(servers.size()) + ")");
    }
}

/// @brief allocates a new server with a random ip and adds it to the pool
void LoadBalancer::addServer() {
    servers.push_back(new WebServer(randomIP()));
    max_servers_seen = max(max_servers_seen, (int)servers.size());
}

/// @brief finds the first idle server, deletes it, and removes it from the pool
void LoadBalancer::removeServer() {
    for (int i = 0; i < (int)servers.size(); i++) {
        if (servers[i]->isFree()) {
            delete servers[i];
            servers.erase(servers.begin() + i);
            min_servers_seen = min(min_servers_seen, (int)servers.size());
            return;
        }
    }
}

/// @brief writes a timestamped message to the log file
void LoadBalancer::log(const string& msg) {
    if (log_file.is_open()) {
        log_file << msg << "\n";
    }
}

/// @brief returns a random dotted-decimal ip address string
string LoadBalancer::randomIP() {
    return to_string(rand() % 256) + "." + to_string(rand() % 256) + "." + to_string(rand() % 256) + "." + to_string(rand() % 256);
}

/// @brief returns the current number of servers in the pool
int LoadBalancer::serverCount() const {
    return (int)servers.size();
}

/// @brief returns the number of requests currently waiting in the queue
int LoadBalancer::queueSize() const {
    return (int)pending.size();
}

/// @brief prints a formatted summary of this load balancers run to stdout
void LoadBalancer::printSummary() {
    cout << CYAN << "--- load balancer [" << job_type << "] summary ---" << RESET << "\n";
    cout << "total requests processed: " << total_processed << "\n";
    cout << "min servers used:         " << min_servers_seen << "\n";
    cout << "max servers used:         " << max_servers_seen << "\n";
    cout << "final queue size:         " << pending.size() << "\n";
    cout << "clock cycles run:         " << clock << "\n";
}
