/// @file request_generator.cpp
/// @brief implements the requestgenerator class

#include "request_generator.h"
#include <cstdlib>
#include <ctime>

RequestGenerator::RequestGenerator() {
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

std::vector<Request> RequestGenerator::generateBatch(int count) {
    std::vector<Request> batch;
    for (int i = 0; i < count; i++) {
        batch.push_back(generate());
    }
    return batch;
}

std::string RequestGenerator::randomIP() {
    return std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256);
}

int RequestGenerator::randomTime() {
    return (rand() % 20) + 1;
}

char RequestGenerator::randomJobType() {
    return (rand() % 2 == 0) ? 'P' : 'S';
}
