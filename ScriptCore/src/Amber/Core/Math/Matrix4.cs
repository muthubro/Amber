using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Amber
{
    [StructLayout(LayoutKind.Explicit)]
    public struct Matrix4
    {
        [FieldOffset(0)]  private Vector4 Col0;
        [FieldOffset(16)] private Vector4 Col1;
        [FieldOffset(32)] private Vector4 Col2;
        [FieldOffset(48)] private Vector4 Col3;

        public Matrix4(float scalar)
        {
            Col0 = new Vector4(scalar, 0.0f, 0.0f, 0.0f);
            Col1 = new Vector4(0.0f, scalar, 0.0f, 0.0f);
            Col2 = new Vector4(0.0f, 0.0f, scalar, 0.0f);
            Col3 = new Vector4(0.0f, 0.0f, 0.0f, scalar);
        }

        public Matrix4(Vector4 col0, Vector4 col1, Vector4 col2, Vector4 col3)
        {
            Col0 = col0;
            Col1 = col1;
            Col2 = col2;
            Col3 = col3;
        }

        public Vector4 this[int index]
        {
            get
            {
                switch (index)
                {
                    case 0: return Col0;
                    case 1: return Col1;
                    case 2: return Col2;
                    case 3: return Col3;
                }

                return Col0;
            }

            set
            {
                switch (index)
                {
                    case 0: Col0 = value; break;
                    case 1: Col1 = value; break;
                    case 2: Col2 = value; break;
                    case 3: Col3 = value; break;
                }
            }
        }

        public static Matrix4 Translate(Vector3 translation)
        {
            Matrix4 result = new Matrix4(1.0f);
            result.Col3.X = translation.X;
            result.Col3.Y = translation.Y;
            result.Col3.Z = translation.Z;
            return result;
        }

        public static Matrix4 Rotate(float angle, Vector3 vector)
        {
            Matrix4 result = new Matrix4(1.0f);

            float c = (float)System.Math.Cos(angle);
            float s = (float)System.Math.Sin(angle);

            Vector3 axis = Vector3.Normalize(vector);
            Vector3 temp = (1.0f - c) * axis;

            result.Col0.X = c + temp.X * axis.X;
            result.Col0.Y = temp.X * axis.Y + s * axis.Z;
            result.Col0.Z = temp.X * axis.Z - s * axis.Y;

            result.Col1.X = temp.Y * axis.X - s * axis.Z;
            result.Col1.Y = c + temp.Y * axis.Y;
            result.Col1.Z = temp.Y * axis.Z + s * axis.X;

            result.Col2.X = temp.Z * axis.X + s * axis.Y;
            result.Col2.Y = temp.Z * axis.Y - s * axis.X;
            result.Col2.Z = c + temp.Z * axis.Z;

            return result;
        }

        public static Matrix4 Scale(Vector3 scale)
        {
            Matrix4 result = new Matrix4(1.0f);
            result.Col0.X = scale.X;
            result.Col1.Y = scale.Y;
            result.Col2.Z = scale.Z;
            return result;
        }
    }
}
