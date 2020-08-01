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
                return GetTag_Native(Entity.ID);
            }

            set
            {
                SetTag_Native(Entity.ID, value);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string GetTag_Native(ulong entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetTag_Native(ulong entityID, string tag);
    }

    public class TransformComponent : Component
    {
        public Matrix4 Transform
        {
            get
            {
                Matrix4 transform;
                GetTransform_Native(Entity.ID, out transform);
                return transform;
            }

            set
            {
                SetTransform_Native(Entity.ID, ref value);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetTransform_Native(ulong entityID, out Matrix4 transform);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetTransform_Native(ulong entityID, ref Matrix4 transform);
    }

    public class MeshComponent : Component
    {
        public Mesh Mesh
        {
            get
            {
                Mesh mesh = new Mesh(GetMesh_Native(Entity.ID));
                return mesh;
            }

            set
            {
                IntPtr ptr = value == null ? IntPtr.Zero : value.m_UnmanagedInstance;
                SetMesh_Native(Entity.ID, ptr);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern IntPtr GetMesh_Native(ulong entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetMesh_Native(ulong entityID, IntPtr mesh);
    }

    public class RigidBody2DComponent : Component
    {
        public void ApplyLinearImpulse(Vector2 impulse, Vector2 offset, bool wake)
        {
            ApplyLinearImpulse_Native(Entity.ID, ref impulse, ref offset, wake);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void ApplyLinearImpulse_Native(ulong entityID, ref Vector2 impulse, ref Vector2 offset, bool wake);
    }
}
