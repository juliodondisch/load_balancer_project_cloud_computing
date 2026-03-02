/// @file request_generator.cpp
/// @brief implements the requestgenerator class

#include "request_generator.h"
#include <cstdlib>
#include <ctime>

using namespace std;

/// @brief seeds the random number generator and stores the time range
RequestGenerator::RequestGenerator(int min_time, int max_time)
    : min_time(min_time), max_time(max_time) {
    srand(time(0));
}

/// @brief builds and returns a single request with random fields
Request RequestGenerator::generate() {
    Request r;
    r.ip_in = randomIP();
    r.ip_out = randomIP();
    r.time = randomTime();
    r.job_type = randomJobType();
    return r;
}

/// @brief generates count random requests and returns them as a vector
vector<Request> RequestGenerator::generateBatch(int count) {
    vector<Request> batch;
    for (int i = 0; i < count; i++) {
        batch.push_back(generate());
    }
    return batch;
}

/// @brief returns a random dotted-decimal ip address string
string RequestGenerator::randomIP() {
    return to_string(rand() % 256) + "." +
           to_string(rand() % 256) + "." +
           to_string(rand() % 256) + "." +
           to_string(rand() % 256);
}

/// @brief returns a random integer between min_time and max_time inclusive
int RequestGenerator::randomTime() {
    return (rand() % (max_time - min_time + 1)) + min_time;
}

/// @brief returns 'P' or 'S' with equal probability
char RequestGenerator::randomJobType() {
    return (rand() % 2 == 0) ? 'P' : 'S';
}
