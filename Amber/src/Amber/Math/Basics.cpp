#include "abpch.h"
#include "Basics.h"

namespace Amber
{
namespace Math
{
int Floor(float value)
{
    return value > 0.0f ? (int)value : (int)value - 1;
}

float Lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

// Quintic-Hermite interpolation
// https://en.wikipedia.org/wiki/Hermite_interpolation#Quintic_Hermite_Interpolation
// with x0 = 0; x1 = 1; f'(x0) = f'(x1) = f"(x0) = f"(x1) = 0
float QHInterpolate(float t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

} // Math
} // Amber