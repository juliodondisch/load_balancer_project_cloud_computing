/// @file high_level_load_balancer.cpp
/// @brief implements the highlevelloadbalancer class

#include "high_level_load_balancer.h"
#include "colors.h"
#include <iostream>

using namespace std;

/// @brief creates both load balancers and stores the blocked ip prefix list
HighLevelLoadBalancer::HighLevelLoadBalancer(int p_servers, int s_servers, int cooldown_n, vector<string> blocked)
    : blocked_prefixes(blocked), total_blocked(0) {
    p_lb = new LoadBalancer('P', p_servers, cooldown_n);
    s_lb = new LoadBalancer('S', s_servers, cooldown_n);
}

/// @brief deletes both load balancers then appends firewall count to each log file
HighLevelLoadBalancer::~HighLevelLoadBalancer() {
    delete p_lb;
    delete s_lb;
    ofstream plog("lb_log_P.txt", ios::app);
    plog << "total requests blocked by firewall: " << total_blocked << "\n";
    plog.close();
    ofstream slog("lb_log_S.txt", ios::app);
    slog << "total requests blocked by firewall: " << total_blocked << "\n";
    slog.close();
}

/// @brief checks the request against the firewall then forwards it to the correct load balancer
void HighLevelLoadBalancer::routeRequest(Request r) {
    for (const string& prefix : blocked_prefixes) {
        if (!prefix.empty() && r.ip_in.substr(0, prefix.size()) == prefix) {
            cout << MAGENTA << "[firewall] blocked request from " << r.ip_in << RESET << "\n";
            total_blocked++;
            return;
        }
    }

    if (r.job_type == 'P') {
        p_lb->addRequest(r);
    } else {
        s_lb->addRequest(r);
    }
}

/// @brief advances both load balancers by one clock cycle
void HighLevelLoadBalancer::tick() {
    p_lb->tick();
    s_lb->tick();
}

/// @brief calls logStartup on both load balancers with the given time range
void HighLevelLoadBalancer::logStartup(int min_time, int max_time) {
    p_lb->logStartup(min_time, max_time);
    s_lb->logStartup(min_time, max_time);
}

/// @brief prints firewall stats and the summary for each load balancer
void HighLevelLoadBalancer::printSummary() {
    cout << BLUE << "\n=== simulation complete ===" << RESET << "\n\n";
    if (total_blocked > 0) {
        cout << MAGENTA << "total requests blocked by firewall: " << total_blocked << RESET << "\n\n";
    }
    p_lb->printSummary();
    cout << "\n";
    s_lb->printSummary();
}
