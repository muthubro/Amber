#pragma once

#include <random>

namespace Amber
{
namespace Math
{

#undef max
#undef min

class Random
{
public:
    static void Init()
    {
        s_RandomEngine.seed(std::random_device()());
    }

    static void Seed(uint32_t seed)
    {
        s_RandomEngine.seed(seed);
    }

    static bool Bool()
    {
        std::bernoulli_distribution dist;
        return dist(s_RandomEngine);
    }

    static float Float()
    {
        std::uniform_real_distribution<float> dist;
        return dist(s_RandomEngine);
    }

    static uint64_t UInt()
    {
        std::uniform_int_distribution<uint64_t> dist;
        return dist(s_RandomEngine);
    }

private:
    inline static std::mt19937 s_RandomEngine;
};

} // Math
} // Amber
