/// @file load_balancer.h
/// @brief declares the loadbalancer class that manages servers and request queues

#ifndef LOAD_BALANCER_H
#define LOAD_BALANCER_H

#include <queue>
#include <vector>
#include <string>
#include <fstream>
#include "request_generator.h"
#include "webserver.h"

/// @brief manages a pool of web servers and a queue of pending requests for one job type
class LoadBalancer {
public:
    /// @brief constructs a load balancer for the given job type with a starting server count
    LoadBalancer(char type, int initial_servers, int cooldown_n);

    /// @brief destructor that writes the summary log and frees all server memory
    ~LoadBalancer();

    /// @brief adds a request to the pending queue
    void addRequest(Request r);

    /// @brief advances the simulation by one clock cycle
    void tick();

    /// @brief prints a summary of this load balancers activity to stdout
    void printSummary();

    /// @brief returns the current number of servers
    int serverCount() const;

    /// @brief returns the current pending queue size
    int queueSize() const;

private:
    char job_type;                  ///< 'P' or 'S', the job type this lb handles
    std::queue<Request> pending;    ///< queue of requests waiting for a server
    std::vector<WebServer*> servers;///< all servers, both busy and free
    int clock;                      ///< current clock cycle
    int adjust_cooldown;            ///< cycles remaining before next scaling check
    int cooldown_period;            ///< cycles to wait between scaling actions
    std::ofstream log_file;         ///< log file for this load balancer

    int total_processed;            ///< total requests completed during the run
    int min_servers_seen;           ///< lowest server count seen during the run
    int max_servers_seen;           ///< highest server count seen during the run

    /// @brief assigns pending requests to any free servers
    void assignJobs();

    /// @brief advances each server by one cycle and logs any completions
    void tickServers();

    /// @brief checks queue depth and adds or removes one server if out of range
    void checkQueueBalance();

    /// @brief adds a new server with a random ip address
    void addServer();

    /// @brief removes one idle server if any are available
    void removeServer();

    /// @brief writes a message to the log file
    void log(const std::string& msg);

    /// @brief generates a random dotted-decimal ip address
    std::string randomIP();
};

#endif
