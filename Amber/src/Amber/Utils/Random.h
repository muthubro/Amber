#pragma once

#include <random>

namespace Amber
{

class Random
{
public:
    static void Init()
    {
        s_RandomEngine.seed(std::random_device()());
    }

    static bool Bool()
    {
        std::bernoulli_distribution dist;
        return dist(s_RandomEngine);
    }

    static float Float()
    {
        std::uniform_real_distribution<float> dist;
        return dist(s_RandomEngine) / (float)std::numeric_limits<float>::max();
    }

private:
    inline static std::mt19937 s_RandomEngine;
    inline static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
};

}
