#pragma once

namespace Amber
{
namespace Math
{

class Noise
{
public:
    static void Init();

    // Noise value in the range [-1, 1]
    static float PerlinNoise(float x, float y, float frequency = 0.01f);
};

} // Math
} // Amber