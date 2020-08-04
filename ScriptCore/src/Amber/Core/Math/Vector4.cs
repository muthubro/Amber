using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Amber
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector4
    {
        public float X;
        public float Y;
        public float Z;
        public float W;

        public Vector4(float scalar)
        {
            X = Y = Z = W = scalar;
        }

        public Vector4(float x, float y, float z, float w)
        {
            X = x;
            Y = y;
            Z = z;
            W = w;
        }

        public Vector4(Vector3 vector)
        {
            X = vector.X;
            Y = vector.Y;
            Z = vector.Z;
            W = 0.0f;
        }

        public Vector4(Vector2 vector)
        {
            X = vector.X;
            Y = vector.Y;
            Z = 0.0f;
            W = 0.0f;
        }

        public Vector3 XYZ
        {
            get { return new Vector3(X, Y, Z); }
            set { X = value.X; Y = value.Y; Z = value.Z; }
        }

        public static Vector4 operator +(Vector4 left, Vector4 right)
        {
            return new Vector4(left.X + right.X, left.Y + right.Y, left.Z + right.Z, left.W + right.W);
        }

        public static Vector4 operator +(Vector4 left, float scalar)
        {
            Vector4 right = new Vector4(scalar);
            return left + right;
        }

        public static Vector4 operator +(float scalar, Vector4 right)
        {
            Vector4 left = new Vector4(scalar);
            return left + right;
        }

        public static Vector4 operator -(Vector4 left, Vector4 right)
        {
            return new Vector4(left.X - right.X, left.Y - right.Y, left.Z - right.Z, left.W - right.W);
        }

        public static Vector4 operator -(Vector4 left, float scalar)
        {
            Vector4 right = new Vector4(scalar);
            return left - right;
        }

        public static Vector4 operator -(float scalar, Vector4 right)
        {
            Vector4 left = new Vector4(scalar);
            return left - right;
        }

        public static Vector4 operator *(Vector4 left, Vector4 right)
        {
            return new Vector4(left.X * right.X, left.Y * right.Y, left.Z * right.Z, left.W * right.W);
        }

        public static Vector4 operator *(Vector4 left, float scalar)
        {
            Vector4 right = new Vector4(scalar);
            return left * right;
        }

        public static Vector4 operator *(float scalar, Vector4 right)
        {
            Vector4 left = new Vector4(scalar);
            return left * right;
        }

        public static Vector4 operator /(Vector4 left, Vector4 right)
        {
            return new Vector4(left.X / right.X, left.Y / right.Y, left.Z / right.Z, left.W / right.W);
        }

        public static Vector4 operator /(Vector4 left, float scalar)
        {
            Vector4 right = new Vector4(scalar);
            return left / right;
        }

        public static Vector4 Normalize(Vector4 vector)
        {
            float mod = (float)System.Math.Sqrt(vector.X * vector.X + vector.Y * vector.Y + vector.Z * vector.Z + vector.W * vector.W);
            return vector / mod;
        }

        public static Vector4 Lerp(Vector4 a, Vector4 b, float t)
        {
            if (t < 0.0f)
                t = 0.0f;
            else if (t > 1.0f)
                t = 1.0f;

            return a + t * (b - a);
        }

        public static Vector4 Clamp(Vector4 vector, Vector4 min, Vector4 max)
        {
            vector = new Vector4(Mathf.Clamp(vector.X, min.X, max.X),
                                 Mathf.Clamp(vector.Y, min.Y, max.Y),
                                 Mathf.Clamp(vector.Z, min.Z, max.Z),
                                 Mathf.Clamp(vector.W, min.W, max.W));
            return vector;
        }
    }
}
