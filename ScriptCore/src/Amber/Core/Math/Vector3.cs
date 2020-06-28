using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Amber
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector3
    {
        public float X;
        public float Y;
        public float Z;

        public Vector3(float scalar)
        {
            X = Y = Z = scalar;
        }

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        public Vector3(Vector4 vector)
        {
            X = vector.X;
            Y = vector.Y;
            Z = vector.Z;
        }

        public static Vector3 operator +(Vector3 left, Vector3 right)
        {
            return new Vector3(left.X + right.X, left.Y + right.Y, left.Z + right.Z);
        }

        public static Vector3 operator +(Vector3 left, float scalar)
        {
            Vector3 right = new Vector3(scalar);
            return left + right;
        }

        public static Vector3 operator +(float scalar, Vector3 right)
        {
            Vector3 left = new Vector3(scalar);
            return left + right;
        }

        public static Vector3 operator -(Vector3 left, Vector3 right)
        {
            return new Vector3(left.X - right.X, left.Y - right.Y, left.Z - right.Z);
        }

        public static Vector3 operator -(Vector3 left, float scalar)
        {
            Vector3 right = new Vector3(scalar);
            return left - right;
        }

        public static Vector3 operator -(float scalar, Vector3 right)
        {
            Vector3 left = new Vector3(scalar);
            return left - right;
        }

        public static Vector3 operator *(Vector3 left, Vector3 right)
        {
            return new Vector3(left.X * right.X, left.Y * right.Y, left.Z * right.Z);
        }

        public static Vector3 operator *(Vector3 left, float scalar)
        {
            Vector3 right = new Vector3(scalar);
            return left * right;
        }

        public static Vector3 operator *(float scalar, Vector3 right)
        {
            Vector3 left = new Vector3(scalar);
            return left * right;
        }

        public static Vector3 operator /(Vector3 left, Vector3 right)
        {
            return new Vector3(left.X / right.X, left.Y / right.Y, left.Z / right.Z);
        }

        public static Vector3 operator /(Vector3 left, float scalar)
        {
            Vector3 right = new Vector3(scalar);
            return left / right;
        }

        public static Vector3 Normalize(Vector3 vector)
        {
            float mod = (float)System.Math.Sqrt(vector.X * vector.X + vector.Y * vector.Y + vector.Z * vector.Z);
            return vector / mod;
        }

        public static Vector3 Lerp(Vector3 a, Vector3 b, float t)
        {
            if (t < 0.0f)
                t = 0.0f;
            else if (t > 1.0f)
                t = 1.0f;

            return a + t * (b - a);
        }
    }
}
