using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Amber
{
    public static class MeshFactory
    {
        public static Mesh CreatePlane(float width, float height)
        {
            return new Mesh(CreatePlane_Native(width, height));
        }

        public static Mesh CreateCube(Vector3 center, float length)
        {
            return new Mesh(CreateCube_Native(ref center, length));
        }

        public static Mesh CreateSphere(Vector3 center, float radius)
        {
            return new Mesh(CreateSphere_Native(ref center, radius));
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern IntPtr CreatePlane_Native(float width, float height);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern IntPtr CreateCube_Native(ref Vector3 center, float length);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern IntPtr CreateSphere_Native(ref Vector3 center, float radius);
    }
}
