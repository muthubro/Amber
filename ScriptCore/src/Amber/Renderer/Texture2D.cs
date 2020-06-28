using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Amber
{
    public class Texture2D
    {
        internal IntPtr m_UnmanagedInstance;

        public Texture2D(uint width, uint height)
        {
            m_UnmanagedInstance = Constructor_Native(width, height);
        }

        ~Texture2D()
        {
            Destructor_Native(m_UnmanagedInstance);
        }

        public void SetData(Vector4[] data)
        {
            SetData_Native(m_UnmanagedInstance, data, data.Length);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern IntPtr Constructor_Native(uint width, uint height);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Destructor_Native(IntPtr unmanagedInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetData_Native(IntPtr unmanagedInstance, Vector4[] data, int size);
    }
}
