/// @file high_level_load_balancer.h
/// @brief declares the highlevelloadbalancer class that routes requests by job type

#ifndef HIGH_LEVEL_LOAD_BALANCER_H
#define HIGH_LEVEL_LOAD_BALANCER_H

#include "load_balancer.h"
#include "request_generator.h"

/// @brief routes incoming requests to the appropriate load balancer based on job type
class HighLevelLoadBalancer {
public:
    /// @brief constructs the high level load balancer with separate server counts per job type
    HighLevelLoadBalancer(int p_servers, int s_servers, int cooldown_n);

    /// @brief destructor that cleans up both load balancers
    ~HighLevelLoadBalancer();

    /// @brief routes the request to the processing or streaming load balancer
    void routeRequest(Request r);

    /// @brief advances both load balancers by one clock cycle
    void tick();

    /// @brief prints a combined summary of both load balancers to stdout
    void printSummary();

private:
    LoadBalancer* p_lb; ///< load balancer that handles processing jobs
    LoadBalancer* s_lb; ///< load balancer that handles streaming jobs
};

#endif
