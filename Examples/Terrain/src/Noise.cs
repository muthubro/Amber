using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Amber;

namespace Terrain
{
    public static class Noise
    {
        public static float[,] GenerateNoiseMap(uint width, uint height, int seed, float scale, uint octaves, float persistance, float lacunarity, Vector2 offset)
        {
            float[,] noiseMap = new float[width, height];

            System.Random random = new System.Random(seed);
            Vector2[] octaveOffsets = new Vector2[octaves];
            for (uint i = 0; i < octaves; i++)
            {
                float offsetX = random.Next(0, 100000) + offset.X;
                float offsetY = random.Next(0, 100000) + offset.Y;
                octaveOffsets[i] = new Vector2(offsetX, offsetY);
            }

            if (scale < 0.0f)
                scale = 0.0f;

            float halfWidth = width * 0.5f;
            float halfHeight = height * 0.5f;

            float minHeight = float.MaxValue;
            float maxHeight = float.MinValue;
            for (uint y = 0; y < height; y++)
            {
                for (uint x = 0; x < width; x++)
                {
                    float amplitude = 1.0f;
                    float frequency = 1.0f;
                    float noiseHeight = 0.0f;

                    for (uint i = 0; i < octaves; i++)
                    {
                        float sampleX = (x - halfWidth) / scale * frequency + octaveOffsets[i].X;
                        float sampleY = (y - halfHeight) / scale * frequency + octaveOffsets[i].Y;
                        noiseHeight += Amber.Noise.PerlinNoise(sampleX, sampleY) * amplitude;

                        amplitude *= persistance;
                        frequency *= lacunarity;
                    }

                    if (noiseHeight > maxHeight)
                        maxHeight = noiseHeight;
                    else if (noiseHeight < minHeight)
                        minHeight = noiseHeight;

                    noiseMap[x, y] = noiseHeight;
                }
            }

            float range = maxHeight - minHeight;
            for (uint y = 0; y < height; y++)
            {
                for (uint x = 0; x < width; x++)
                {
                    noiseMap[x, y] = (noiseMap[x, y] - minHeight) / range;
                }
            }

            return noiseMap;
        }
    }
}
