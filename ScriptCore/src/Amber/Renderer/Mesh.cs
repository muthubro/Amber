using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Amber
{
    public class Mesh
    {
        internal IntPtr m_UnmanagedInstance;

        public Mesh(string filepath)
        {
            m_UnmanagedInstance = Constructor_Native(filepath);
        }

        internal Mesh(IntPtr unmanagedInstance)
        {
            m_UnmanagedInstance = unmanagedInstance;
        }

        ~Mesh()
        {
            Destructor_Native(m_UnmanagedInstance);
        }

        public Material GetMaterial()
        {
            return new Material(GetMaterial_Native(m_UnmanagedInstance));
        }

        public MaterialInstance GetMaterialByIndex(uint index)
        {
            return new MaterialInstance(GetMaterialByIndex_Native(m_UnmanagedInstance, index));
        }

        public uint GetMaterialCount()
        {
            return GetMaterialCount_Native(m_UnmanagedInstance);
        }

        public uint GetSubmeshCount()
        {
            return GetSubmeshCount_Native(m_UnmanagedInstance);
        }

        public void SetAlbedoTexture(uint submeshIndex, bool use, Texture2D albedo)
        {
            SetAlbedoTexture_Native(m_UnmanagedInstance, submeshIndex, use, albedo.m_UnmanagedInstance);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern IntPtr Constructor_Native(string filepath);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Destructor_Native(IntPtr unmanagedInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern IntPtr GetMaterial_Native(IntPtr unmanagedInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern IntPtr GetMaterialByIndex_Native(IntPtr unmanagedInstance, uint index);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern uint GetMaterialCount_Native(IntPtr unmanagedInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern uint GetSubmeshCount_Native(IntPtr unmanagedInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetAlbedoTexture_Native(IntPtr unmanagedInstance, uint submeshIndex, bool use, IntPtr albedo);
    }
}
