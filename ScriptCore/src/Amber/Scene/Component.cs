using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Amber
{
    public abstract class Component
    {
        public Entity Entity { get; set; }
    }

    public class TagComponent : Component
    {
        public string Tag
        {
            get
            {
                return GetTag_Native(Entity.SceneID, Entity.EntityID);
            }

            set
            {
                SetTag_Native(Entity.SceneID, Entity.EntityID, value);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string GetTag_Native(uint sceneID, uint entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetTag_Native(uint sceneID, uint entityID, string tag);
    }

    public class TransformComponent : Component
    {
        public Matrix4 Transform
        {
            get
            {
                Matrix4 transform;
                GetTransform_Native(Entity.SceneID, Entity.EntityID, out transform);
                return transform;
            }

            set
            {
                SetTransform_Native(Entity.SceneID, Entity.EntityID, ref value);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetTransform_Native(uint sceneID, uint entityID, out Matrix4 transform);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetTransform_Native(uint sceneID, uint entityID, ref Matrix4 transform);
    }

    public class MeshComponent : Component
    {
        public Mesh Mesh
        {
            get
            {
                Mesh mesh = new Mesh(GetMesh_Native(Entity.SceneID, Entity.EntityID));
                return mesh;
            }

            set
            {
                IntPtr ptr = value == null ? IntPtr.Zero : value.m_UnmanagedInstance;
                SetMesh_Native(Entity.SceneID, Entity.EntityID, ptr);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern IntPtr GetMesh_Native(uint sceneID, uint entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetMesh_Native(uint sceneID, uint entityID, IntPtr mesh);
    }
}
