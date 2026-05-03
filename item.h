#pragma once

#include<cstdint>


struct item
{
    uint64_t push_ns; // timestamp occured
    uint64_t seq; // counter
    char payload[64];

};