/// @file main.cpp
/// @brief entry point for the load balancer simulation

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>
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

/// @brief extracts the clock number from a log line, returns -1 if not a clock event line
int clockOf(const string& line) {
    size_t pos = line.find("[clock ");
    if (pos == string::npos) return -1;
    size_t end = line.find("]", pos + 7);
    if (end == string::npos) return -1;
    try { return stoi(line.substr(pos + 7, end - pos - 7)); }
    catch (...) { return -1; }
}

/// @brief reads lines of a file into a vector
vector<string> readLines(const string& filename) {
    vector<string> lines;
    ifstream f(filename);
    string line;
    while (getline(f, line)) lines.push_back(line);
    return lines;
}

/// @brief parses an integer value from "key: N" within the last 10 lines of a log
int parseStat(const vector<string>& lines, const string& key) {
    int start = max(0, (int)lines.size() - 10);
    for (int i = start; i < (int)lines.size(); i++) {
        if (lines[i].find(key) != string::npos) {
            size_t pos = lines[i].find(": ");
            if (pos != string::npos) {
                try { return stoi(lines[i].substr(pos + 2)); } catch (...) {}
            }
        }
    }
    return 0;
}

/// @brief writes the last 10 lines of a log file (the summary section) to an output stream
void writeSummarySection(ofstream& out, const vector<string>& lines) {
    int start = max(0, (int)lines.size() - 10);
    for (int i = start; i < (int)lines.size(); i++) out << lines[i] << "\n";
}

/// @brief writes a combined summary of both load balancers to an output stream
void writeConjoinedSummary(ofstream& out, const vector<string>& p, const vector<string>& s) {
    int p_proc    = parseStat(p, "total requests processed");
    int s_proc    = parseStat(s, "total requests processed");
    int p_min     = parseStat(p, "min servers used");
    int s_min     = parseStat(s, "min servers used");
    int p_max     = parseStat(p, "max servers used");
    int s_max     = parseStat(s, "max servers used");
    int p_active  = parseStat(p, "active servers at end");
    int s_active  = parseStat(s, "active servers at end");
    int p_idle    = parseStat(p, "idle servers at end");
    int s_idle    = parseStat(s, "idle servers at end");
    int p_queue   = parseStat(p, "final queue size");
    int s_queue   = parseStat(s, "final queue size");
    int cycles    = parseStat(p, "total clock cycles run");
    int blocked   = parseStat(p, "total requests blocked by firewall");

    out << "--- conjoined summary ---\n";
    out << "total requests processed: " << (p_proc + s_proc) << " (lb-P: " << p_proc << ", lb-S: " << s_proc << ")\n";
    out << "min servers used:         lb-P: " << p_min << ", lb-S: " << s_min << "\n";
    out << "max servers used:         lb-P: " << p_max << ", lb-S: " << s_max << "\n";
    out << "active servers at end:    " << (p_active + s_active) << " (lb-P: " << p_active << ", lb-S: " << s_active << ")\n";
    out << "idle servers at end:      " << (p_idle + s_idle) << " (lb-P: " << p_idle << ", lb-S: " << s_idle << ")\n";
    out << "final queue size:         " << (p_queue + s_queue) << " (lb-P: " << p_queue << ", lb-S: " << s_queue << ")\n";
    out << "clock cycles run:         " << cycles << "\n";
    out << "total requests blocked by firewall: " << blocked << "\n";
}

/// @brief merges lb_log_P.txt and lb_log_S.txt into lb_log.txt
/// structure: startup headers from both, events sorted by clock, individual summaries, conjoined summary
void mergeLogs(const vector<string>& p, const vector<string>& s) {
    ofstream out("lb_log.txt");

    // startup headers: first 7 lines of each file
    const int HEADER = 7;
    out << "=== startup ===\n";
    for (int i = 0; i < HEADER && i < (int)p.size(); i++) out << p[i] << "\n";
    for (int i = 0; i < HEADER && i < (int)s.size(); i++) out << s[i] << "\n";

    // collect all clock events from both files and sort by clock number
    vector<pair<int, string>> events;
    for (const vector<string>* log : {&p, &s}) {
        for (int i = HEADER; i < (int)log->size(); i++) {
            int c = clockOf((*log)[i]);
            if (c >= 0) events.push_back({c, (*log)[i]});
        }
    }
    stable_sort(events.begin(), events.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });
    out << "\n=== events ===\n";
    for (const auto& [c, line] : events) out << line << "\n";

    // individual summaries then conjoined summary
    out << "\n=== summaries ===\n";
    writeSummarySection(out, p);
    out << "\n";
    writeSummarySection(out, s);
    out << "\n";
    writeConjoinedSummary(out, p, s);
}

/// @brief writes lb_log_summary.txt with startup messages, individual summaries, and conjoined summary
void writeSummaryLog(const vector<string>& p, const vector<string>& s) {
    ofstream out("lb_log_summary.txt");

    out << "=== startup ===\n";
    if (!p.empty()) out << p[0] << "\n";
    if (!s.empty()) out << s[0] << "\n";

    out << "\n=== summaries ===\n";
    writeSummarySection(out, p);
    out << "\n";
    writeSummarySection(out, s);
    out << "\n";
    writeConjoinedSummary(out, p, s);
}

/// @brief sets up the simulation from config and user input then runs the clock loop
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

    {
        HighLevelLoadBalancer hlbt(p_servers, s_servers, cooldown, blocked_ips);

        int initial_count = num_servers * 100;
        cout << GREEN << "generating " << initial_count << " initial requests..." << RESET << "\n";

        vector<Request> batch = gen.generateBatch(initial_count);
        for (Request& r : batch) {
            hlbt.routeRequest(r);
        }

        hlbt.logStartup(min_request_time, max_request_time);
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
    } // hlbt destructor runs here, flushing summaries and firewall count to log files

    vector<string> p_lines = readLines("lb_log_P.txt");
    vector<string> s_lines = readLines("lb_log_S.txt");
    mergeLogs(p_lines, s_lines);
    writeSummaryLog(p_lines, s_lines);
    cout << "\n" << CYAN << "logs written to lb_log_P.txt, lb_log_S.txt, lb_log.txt, lb_log_summary.txt" << RESET << "\n";

    return 0;
}
