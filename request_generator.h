/// @file request_generator.h
/// @brief declares the requestgenerator class that creates random requests

#ifndef REQUEST_GENERATOR_H
#define REQUEST_GENERATOR_H

#include <vector>
#include <string>

/// @brief holds all data for a single web request
struct Request {
    std::string ip_in;
    std::string ip_out;
    int time;
    char job_type;
};

/// @brief generates random web requests for use in the simulation
class RequestGenerator {
public:
    /// @brief constructs the generator and seeds the random number generator
    RequestGenerator();

    /// @brief generates a single random request
    Request generate();

    /// @brief generates a batch of count random requests
    std::vector<Request> generateBatch(int count);

private:
    /// @brief generates a random dotted-decimal ip address
    std::string randomIP();

    /// @brief generates a random processing time between 1 and 20 clock cycles
    int randomTime();

    /// @brief returns 'P' or 'S' with equal probability
    char randomJobType();
};

#endif
