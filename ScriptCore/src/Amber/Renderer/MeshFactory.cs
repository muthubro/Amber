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
        public static Mesh CreatePlane(uint width, uint height)
        {
            return new Mesh(CreatePlane_Native(width, height));
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern IntPtr CreatePlane_Native(uint width, uint height);
    }
}
