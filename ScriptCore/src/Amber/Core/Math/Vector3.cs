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

        public Vector3(Vector2 vector)
        {
            X = vector.X;
            Y = vector.Y;
            Z = 0.0f;
        }

        public Vector2 XY
        {
            get { return new Vector2(X, Y); }
            set { X = value.X; Y = value.Y; }
        }

        public Vector2 XZ
        {
            get { return new Vector2(X, Z); }
            set { X = value.X; Z = value.Y; }
        }

        public Vector2 YZ
        {
            get { return new Vector2(Y, Z); }
            set { Y = value.X; Z = value.Y; }
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

        public static Vector3 Clamp(Vector3 vector, Vector3 min, Vector3 max)
        {
            vector = new Vector3(Mathf.Clamp(vector.X, min.X, max.X),
                                 Mathf.Clamp(vector.Y, min.Y, max.Y),
                                 Mathf.Clamp(vector.Z, min.Z, max.Z));
            return vector;
        }
    }
}
