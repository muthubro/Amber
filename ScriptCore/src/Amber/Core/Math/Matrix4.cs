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

        public static Matrix4 operator +(Matrix4 left, Matrix4 right)
        {
            return new Matrix4(left.Col0 + right.Col0, 
                left.Col1 + right.Col1, 
                left.Col2 + right.Col2, 
                left.Col3 + right.Col3);
        }

        public static Matrix4 operator -(Matrix4 left, Matrix4 right)
        {
            return new Matrix4(left.Col0 - right.Col0, 
                left.Col1 - right.Col1, 
                left.Col2 - right.Col2, 
                left.Col3 - right.Col3);
        }

        public static Matrix4 operator *(Matrix4 left, float scalar)
        {
            return new Matrix4(left.Col0 * scalar, 
                left.Col1 * scalar, 
                left.Col2 * scalar, 
                left.Col3 * scalar);
        }

        public static Matrix4 operator *(float scalar, Matrix4 right)
        {
            return new Matrix4(right.Col0 * scalar, 
                right.Col1 * scalar, 
                right.Col2 * scalar, 
                right.Col3 * scalar);
        }

        public static Vector4 operator *(Matrix4 left, Vector4 right)
        {
            return left.Col0 * right.X + left.Col1 + right.Y + left.Col2 * right.Z + left.Col3 * right.W;
        }

        public static Matrix4 operator *(Matrix4 left, Matrix4 right)
        {
            Vector4 A0 = left.Col0;
            Vector4 A1 = left.Col1;
            Vector4 A2 = left.Col2;
            Vector4 A3 = left.Col3;

            Vector4 B0 = right.Col0;
            Vector4 B1 = right.Col1;
            Vector4 B2 = right.Col2;
            Vector4 B3 = right.Col3;

            Matrix4 result = new Matrix4(0.0f);
            result.Col0 = A0 * B0.X + A1 * B0.Y + A2 * B0.Z + A3 * B0.W;
            result.Col1 = A0 * B1.X + A1 * B1.Y + A2 * B1.Z + A3 * B1.W;
            result.Col2 = A0 * B2.X + A1 * B2.Y + A2 * B2.Z + A3 * B2.W;
            result.Col3 = A0 * B3.X + A1 * B3.Y + A2 * B3.Z + A3 * B3.W;

            return result;
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
