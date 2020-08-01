using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Amber;

namespace Terrain
{
    public static class TextureGenerator
    {
        public static Texture2D TextureFromColorMap(Color[] colorMap, uint width, uint height)
        {
            Texture2D texture = new Texture2D(width, height);
            texture.SetData(colorMap);
            return texture;
        }

        public static Texture2D TextureFromHeightMap(float[,] heightMap)
        {
            uint width = (uint)heightMap.GetLength(0);
            uint height = (uint)heightMap.GetLength(1);

            Color[] colorMap = new Color[width * height];
            for (uint y = 0; y < height; y++)
            {
                for (uint x = 0; x < width; x++)
                {
                    colorMap[y * width + x] = Color.Lerp(Color.Black, Color.White, heightMap[x, y]);
                }
            }

            return TextureFromColorMap(colorMap, width, height);
        }
    }
}
