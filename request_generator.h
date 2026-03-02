/// @file request_generator.h
/// @brief declares the request struct and requestgenerator class

#ifndef REQUEST_GENERATOR_H
#define REQUEST_GENERATOR_H

#include <vector>
#include <string>

using namespace std;

/// @brief holds all data for a single web request
struct Request {
    string ip_in;   ///< source ip address of the request
    string ip_out;  ///< destination ip address for the response
    int time;       ///< number of clock cycles needed to process this request
    char job_type;  ///< job type: 'P' for processing, 'S' for streaming
};

/// @brief generates random web requests for use in the simulation
class RequestGenerator {
public:
    /// @brief constructs the generator with configurable time range
    RequestGenerator(int min_time = 1, int max_time = 20);

    /// @brief generates a single random request
    Request generate();

    /// @brief generates a batch of count random requests
    vector<Request> generateBatch(int count);

private:
    int min_time; ///< minimum clock cycles a request can take
    int max_time; ///< maximum clock cycles a request can take

    /// @brief generates a random dotted-decimal ip address
    string randomIP();

    /// @brief generates a random processing time between min_time and max_time
    int randomTime();

    /// @brief returns 'P' or 'S' with equal probability
    char randomJobType();
};

#endif
