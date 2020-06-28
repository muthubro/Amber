using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Terrain
{
    public static class Noise
    {
        public static float[,] GenerateNoiseMap(uint width, uint height, float scale)
        {
            float[,] noiseMap = new float[width, height];

            if (scale < 0.0f)
                scale = 0.0f;

            for (uint y = 0; y < height; y++)
            {
                for (uint x = 0; x < width; x++)
                {
                    float sampleX = x / scale + 0.1f;
                    float sampleY = y / scale + 0.1f;
                    noiseMap[x, y] = Amber.Noise.PerlinNoise(sampleX, sampleY);
                }
            }

            return noiseMap;
        }
    }
}
