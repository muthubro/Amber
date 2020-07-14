using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Amber;
using Amber.Editor;

namespace Terrain
{
    public class MapGenerator : Entity
    {
        [EditorField("Width", 1, 2048, 1)]
        public uint width = 512;

        [EditorField("Height", 1, 2048, 1)]
        public uint height = 512;

        [EditorField("Scale", 0.01f, 10.0f, 0.01f)]
        public float scale = 0.3f;

        private void GenerateMap()
        {
            float[,] noiseMap = Noise.GenerateNoiseMap(width, height, scale);

            Texture2D texture = new Texture2D(width, height);
            Vector4[] colorMap = new Vector4[width * height];
            for (uint y = 0; y < height; y++)
            {
                for (uint x = 0; x < width; x++)
                {
                    colorMap[y * width + x] = Vector4.Lerp(Color.Black, Color.White, noiseMap[x, y]);
                }
            }

            texture.SetData(colorMap);

            MeshComponent meshComponent = GetComponent<MeshComponent>();
            if (meshComponent == null)
                meshComponent = CreateComponent<MeshComponent>();
            meshComponent.Mesh = MeshFactory.CreatePlane(width, height);
            meshComponent.Mesh.SetAlbedoTexture(0, true, texture);
        }

        public void OnCreate()
        {
            GenerateMap();
        }
    }
}
