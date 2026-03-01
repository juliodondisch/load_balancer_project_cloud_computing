/// @file high_level_load_balancer.cpp
/// @brief implements the highlevelloadbalancer class

#include "high_level_load_balancer.h"
#include <iostream>

HighLevelLoadBalancer::HighLevelLoadBalancer(int p_servers, int s_servers, int cooldown_n) {
    p_lb = new LoadBalancer('P', p_servers, cooldown_n);
    s_lb = new LoadBalancer('S', s_servers, cooldown_n);
}

HighLevelLoadBalancer::~HighLevelLoadBalancer() {
    delete p_lb;
    delete s_lb;
}

void HighLevelLoadBalancer::routeRequest(Request r) {
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
    std::cout << "\n=== simulation complete ===\n\n";
    p_lb->printSummary();
    std::cout << "\n";
    s_lb->printSummary();
}
