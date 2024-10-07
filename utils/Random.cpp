#include "Random.h"
#include <random>

uint8_t Random::rand_u8(const uint8_t min, const uint8_t max)
{
    static std::random_device       rd;
    static std::mt19937             gen(rd());

    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

