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

        [EditorField("Seed", -1000.0f, 1000.0f, 1.0f)]
        public int seed = 42;

        [EditorField("Scale", 0.01f, 10.0f, 0.01f)]
        public float scale = 0.3f;

        [EditorField("Octaves", 0.0f, 10.0f, 1.0f)]
        public uint octaves = 1;

        [EditorField("Persistance", 0.0f, 1.0f, 0.01f)]
        public float persistance = 0.5f;

        [EditorField("Lacunarity", 1.0f, 10000.0f, 0.1f)]
        public float lacunarity = 2.0f;

        [EditorField("Offset", 0.0f, 10000.0f, 1.0f)]
        public Vector2 offset = new Vector2(0.0f, 0.0f);

        private void GenerateMap()
        {
            float[,] noiseMap = Noise.GenerateNoiseMap(width, height, seed, scale, octaves, persistance, lacunarity, offset);
            Texture2D texture = TextureGenerator.TextureFromHeightMap(noiseMap);
            MeshComponent meshComponent = GetComponent<MeshComponent>();
            meshComponent.Mesh.SetAlbedoTexture(true, texture);
        }

        public void OnCreate()
        {
            GenerateMap();
        }
    }
}
