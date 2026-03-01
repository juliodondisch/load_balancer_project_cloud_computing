/// @file main.cpp
/// @brief entry point for the load balancer simulation

#include <iostream>
#include <cstdlib>
#include "request_generator.h"
#include "high_level_load_balancer.h"

int main() {
    int num_servers, run_time;

    std::cout << "enter number of servers: ";
    std::cin >> num_servers;
    std::cout << "enter number of clock cycles to run: ";
    std::cin >> run_time;

    if (num_servers < 2) {
        num_servers = 2;
        std::cout << "minimum 2 servers required, using 2\n";
    }

    RequestGenerator gen;

    int p_servers = num_servers / 2;
    int s_servers = num_servers - p_servers;

    HighLevelLoadBalancer hlbt(p_servers, s_servers, 20);

    int initial_count = num_servers * 100;
    std::cout << "generating " << initial_count << " initial requests...\n";

    std::vector<Request> batch = gen.generateBatch(initial_count);
    for (Request& r : batch) {
        hlbt.routeRequest(r);
    }

    std::cout << "running simulation for " << run_time << " clock cycles...\n";

    for (int cycle = 0; cycle < run_time; cycle++) {
        // roughly 30% chance per cycle to add 1-3 new requests
        if (rand() % 100 < 30) {
            int new_count = (rand() % 3) + 1;
            for (int i = 0; i < new_count; i++) {
                hlbt.routeRequest(gen.generate());
            }
        }
        hlbt.tick();
    }

    hlbt.printSummary();
    std::cout << "\nlog files written to lb_log_P.txt and lb_log_S.txt\n";

    return 0;
}
