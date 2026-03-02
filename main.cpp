/// @file main.cpp
/// @brief entry point for the load balancer simulation

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <cstdlib>
#include "colors.h"
#include "request_generator.h"
#include "high_level_load_balancer.h"

using namespace std;

/// @brief reads key=value pairs from a config file into a map
map<string, string> readConfig(const string& filename) {
    map<string, string> cfg;
    ifstream file(filename);
    if (!file.is_open()) {
        cout << YELLOW << "config.txt not found, using defaults" << RESET << "\n";
        return cfg;
    }
    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        size_t pos = line.find('=');
        if (pos != string::npos) {
            cfg[line.substr(0, pos)] = line.substr(pos + 1);
        }
    }
    return cfg;
}

/// @brief splits a comma-separated string into a vector of trimmed tokens
vector<string> splitCSV(const string& s) {
    vector<string> tokens;
    stringstream ss(s);
    string token;
    while (getline(ss, token, ',')) {
        if (!token.empty()) tokens.push_back(token);
    }
    return tokens;
}

int main() {
    map<string, string> cfg = readConfig("config.txt");

    // helper lambda to get int from config with a default
    auto getInt = [&](const string& key, int def) {
        return cfg.count(key) && !cfg[key].empty() ? stoi(cfg[key]) : def;
    };

    int cooldown          = getInt("cooldown_period",         20);
    int min_request_time  = getInt("min_request_time",         1);
    int max_request_time  = getInt("max_request_time",        20);
    int new_req_prob      = getInt("new_request_probability", 30);
    int max_new_per_cycle = getInt("max_new_per_cycle",        3);

    vector<string> blocked_ips;
    if (cfg.count("blocked_ips") && !cfg["blocked_ips"].empty()) {
        blocked_ips = splitCSV(cfg["blocked_ips"]);
    }

    int num_servers, run_time;
    cout << "enter number of servers: ";
    cin >> num_servers;
    cout << "enter number of clock cycles to run: ";
    cin >> run_time;

    if (num_servers < 2) {
        cout << RED << "error: minimum of 2 servers required" << RESET << "\n";
        return 1;
    }

    cout << CYAN << "\nconfig loaded:" << RESET << "\n";
    cout << "  cooldown period:      " << cooldown << " cycles\n";
    cout << "  request time range:   " << min_request_time << "-" << max_request_time << " cycles\n";
    cout << "  new request chance:   " << new_req_prob << "% per cycle\n";
    cout << "  max new per cycle:    " << max_new_per_cycle << "\n";
    cout << "  blocked ip prefixes:  " << (blocked_ips.empty() ? "none" : cfg["blocked_ips"]) << "\n\n";

    RequestGenerator gen(min_request_time, max_request_time);

    int p_servers = num_servers / 2;
    int s_servers = num_servers - p_servers;

    HighLevelLoadBalancer hlbt(p_servers, s_servers, cooldown, blocked_ips);

    int initial_count = num_servers * 100;
    cout << GREEN << "generating " << initial_count << " initial requests..." << RESET << "\n";

    vector<Request> batch = gen.generateBatch(initial_count);
    for (Request& r : batch) {
        hlbt.routeRequest(r);
    }

    cout << GREEN << "running simulation for " << run_time << " clock cycles...\n" << RESET;

    for (int cycle = 0; cycle < run_time; cycle++) {
        if (rand() % 100 < new_req_prob) {
            int new_count = (rand() % max_new_per_cycle) + 1;
            for (int i = 0; i < new_count; i++) {
                hlbt.routeRequest(gen.generate());
            }
        }
        hlbt.tick();
    }

    hlbt.printSummary();
    cout << "\n" << CYAN << "log files written to lb_log_P.txt and lb_log_S.txt" << RESET << "\n";

    return 0;
}
