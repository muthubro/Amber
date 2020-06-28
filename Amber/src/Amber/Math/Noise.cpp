#include "abpch.h"
#include "Noise.h"

#include "Amber/Core/Base.h"

#include "Amber/Math/Basics.h"
#include "Amber/Math/Random.h"

namespace Amber
{
namespace Math
{

static const float s_GradX[] = {
     1.0f,  0.9238795325f,  0.7071067812f,  0.3826834324f,
     0.0f, -0.3826834324f, -0.7071067812f, -0.9238795325f,
    -1.0f, -0.9238795325f, -0.7071067812f, -0.3826834324f,
     0.0f,  0.3826834324f,  0.7071067812f,  0.9238795325f,
};

static const float s_GradY[] = {
     0.0f,  0.3826834324f,  0.7071067812f,  0.9238795325f,
     1.0f,  0.9238795325f,  0.7071067812f,  0.3826834324f,
     0.0f, -0.3826834324f, -0.7071067812f, -0.9238795325f,
    -1.0f, -0.9238795325f, -0.7071067812f, -0.3826834324f,
};

static uint32_t s_Perm[512];
static uint32_t s_Perm16[512];

static uint32_t GetGradIndex(uint32_t x, uint32_t y)
{
    return s_Perm16[(x & 0xff) + s_Perm[y & 0xff]];
}

static float DistanceDotGrad(float dx, float dy, uint32_t x, uint32_t y)
{
    uint32_t gradIndex = GetGradIndex(x, y);
    return dx * s_GradX[gradIndex] + dy * s_GradY[gradIndex];
}

void Noise::Init()
{
    for (uint32_t i = 0; i < 256; i++)
        s_Perm[i] = i;

    for (uint32_t i = 0; i < 256; i++)
    {
        uint32_t range = (uint32_t)(Random::Float() * (256 - i));
        uint32_t j = range + i;
        uint32_t temp = s_Perm[i];
        s_Perm[i] = s_Perm[i + 256] = s_Perm[j];
        s_Perm[j] = temp;
        s_Perm16[i] = s_Perm16[i + 256] = s_Perm[i] % 16;
    }
}

float Noise::PerlinNoise(float x, float y, float frequency)
{
    if (x < 0.0f)
        x = 0.0f;
    if (y < 0.0f)
        y = 0.0f;

    x *= frequency;
    y *= frequency;

    uint32_t x0 = (uint32_t)Floor(x);
    uint32_t y0 = (uint32_t)Floor(y);
    uint32_t x1 = x0 + 1;
    uint32_t y1 = y0 + 1;

    float dx = x - x0;
    float dy = y - y0;

    float t_x = QHInterpolate(dx);
    float t_y = QHInterpolate(dy);

    float noise =
        Lerp(
            Lerp(DistanceDotGrad(dx, dy, x0, y0)       , DistanceDotGrad(dx - 1.0f, dy, x1, y0)       , t_x),
            Lerp(DistanceDotGrad(dx, dy - 1.0f, x0, y1), DistanceDotGrad(dx - 1.0f, dy - 1.0f, x1, y1), t_x),
            t_y
        );

    return noise;
}

} // Math
} // Amber