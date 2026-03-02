/// @file request_generator.cpp
/// @brief implements the requestgenerator class

#include "request_generator.h"
#include <cstdlib>
#include <ctime>

using namespace std;

RequestGenerator::RequestGenerator(int min_time, int max_time)
    : min_time(min_time), max_time(max_time) {
    srand(time(0));
}

Request RequestGenerator::generate() {
    Request r;
    r.ip_in = randomIP();
    r.ip_out = randomIP();
    r.time = randomTime();
    r.job_type = randomJobType();
    return r;
}

vector<Request> RequestGenerator::generateBatch(int count) {
    vector<Request> batch;
    for (int i = 0; i < count; i++) {
        batch.push_back(generate());
    }
    return batch;
}

string RequestGenerator::randomIP() {
    return to_string(rand() % 256) + "." +
           to_string(rand() % 256) + "." +
           to_string(rand() % 256) + "." +
           to_string(rand() % 256);
}

int RequestGenerator::randomTime() {
    return (rand() % (max_time - min_time + 1)) + min_time;
}

char RequestGenerator::randomJobType() {
    return (rand() % 2 == 0) ? 'P' : 'S';
}
