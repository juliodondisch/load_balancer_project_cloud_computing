/// @file high_level_load_balancer.cpp
/// @brief implements the highlevelloadbalancer class

#include "high_level_load_balancer.h"
#include "colors.h"
#include <iostream>

using namespace std;

HighLevelLoadBalancer::HighLevelLoadBalancer(int p_servers, int s_servers, int cooldown_n, vector<string> blocked)
    : blocked_prefixes(blocked), total_blocked(0) {
    p_lb = new LoadBalancer('P', p_servers, cooldown_n);
    s_lb = new LoadBalancer('S', s_servers, cooldown_n);
}

HighLevelLoadBalancer::~HighLevelLoadBalancer() {
    delete p_lb;
    delete s_lb;
}

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

void HighLevelLoadBalancer::tick() {
    p_lb->tick();
    s_lb->tick();
}

void HighLevelLoadBalancer::printSummary() {
    cout << BLUE << "\n=== simulation complete ===" << RESET << "\n\n";
    if (total_blocked > 0) {
        cout << MAGENTA << "total requests blocked by firewall: " << total_blocked << RESET << "\n\n";
    }
    p_lb->printSummary();
    cout << "\n";
    s_lb->printSummary();
}
