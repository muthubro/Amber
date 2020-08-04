using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Amber
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector2
    {
        public float X;
        public float Y;

        public Vector2(float scalar)
        {
            X = Y = scalar;
        }

        public Vector2(float x, float y)
        {
            X = x;
            Y = y;
        }

        public Vector2(Vector3 vector)
        {
            X = vector.X;
            Y = vector.Y;
        }

        public Vector2(Vector4 vector)
        {
            X = vector.X;
            Y = vector.Y;
        }

        public static Vector2 operator +(Vector2 left, Vector2 right)
        {
            return new Vector2(left.X + right.X, left.Y + right.Y);
        }

        public static Vector2 operator +(Vector2 left, float scalar)
        {
            Vector2 right = new Vector2(scalar);
            return left + right;
        }

        public static Vector2 operator +(float scalar, Vector2 right)
        {
            Vector2 left = new Vector2(scalar);
            return left + right;
        }

        public static Vector2 operator -(Vector2 left, Vector2 right)
        {
            return new Vector2(left.X - right.X, left.Y - right.Y);
        }

        public static Vector2 operator -(Vector2 left, float scalar)
        {
            Vector2 right = new Vector2(scalar);
            return left - right;
        }

        public static Vector2 operator -(float scalar, Vector2 right)
        {
            Vector2 left = new Vector2(scalar);
            return left - right;
        }

        public static Vector2 operator *(Vector2 left, Vector2 right)
        {
            return new Vector2(left.X * right.X, left.Y * right.Y);
        }

        public static Vector2 operator *(Vector2 left, float scalar)
        {
            Vector2 right = new Vector2(scalar);
            return left * right;
        }

        public static Vector2 operator *(float scalar, Vector2 right)
        {
            Vector2 left = new Vector2(scalar);
            return left * right;
        }

        public static Vector2 operator /(Vector2 left, Vector2 right)
        {
            return new Vector2(left.X / right.X, left.Y / right.Y);
        }

        public static Vector2 operator /(Vector2 left, float scalar)
        {
            Vector2 right = new Vector2(scalar);
            return left / right;
        }

        public static Vector2 Normalize(Vector2 vector)
        {
            float mod = (float)System.Math.Sqrt(vector.X * vector.X + vector.Y * vector.Y);
            return vector / mod;
        }

        public static Vector2 Lerp(Vector2 a, Vector2 b, float t)
        {
            if (t < 0.0f)
                t = 0.0f;
            else if (t > 1.0f)
                t = 1.0f;

            return a + t * (b - a);
        }

        public static Vector2 Clamp(Vector2 vector, Vector2 min, Vector2 max)
        {
            vector = new Vector2(Mathf.Clamp(vector.X, min.X, max.X), 
                                 Mathf.Clamp(vector.Y, min.Y, max.Y));
            return vector;
        }
    }
}
