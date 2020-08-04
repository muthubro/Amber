using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Amber
{
    public struct Color
    {
        public float R;
        public float G;
        public float B;
        public float A;

        public Color(float scalar)
        {
            R = G = B = A = scalar;
        }

        public Color(float r, float g, float b, float a)
        {
            R = r > 1.0f ? 1.0f : (r < 0.0f ? 0.0f : r);
            G = g > 1.0f ? 1.0f : (g < 0.0f ? 0.0f : g);
            B = b > 1.0f ? 1.0f : (b < 0.0f ? 0.0f : b);
            A = a > 1.0f ? 1.0f : (a < 0.0f ? 0.0f : a);
        }

        public Color(float r, float g, float b)
        {
            R = r > 1.0f ? 1.0f : (r < 0.0f ? 0.0f : r);
            G = g > 1.0f ? 1.0f : (g < 0.0f ? 0.0f : g);
            B = b > 1.0f ? 1.0f : (b < 0.0f ? 0.0f : b);
            A = 1.0f;
        }

        public Color(Vector4 vector)
        {
            R = vector.X > 1.0f ? 1.0f : (vector.X < 0.0f ? 0.0f : vector.X);
            G = vector.Y > 1.0f ? 1.0f : (vector.Y < 0.0f ? 0.0f : vector.Y);
            B = vector.Z > 1.0f ? 1.0f : (vector.Z < 0.0f ? 0.0f : vector.Z);
            A = vector.W > 1.0f ? 1.0f : (vector.W < 0.0f ? 0.0f : vector.W);
        }

        public static Color operator +(Color left, Color right)
        {
            return new Color(left.R + right.R, left.G + right.G, left.B + right.B, left.A + right.A);
        }

        public static Color operator -(Color left, Color right)
        {
            return new Color(left.R - right.R, left.G - right.G, left.B - right.B, left.A - right.A);
        }

        public static Color operator *(Color left, float scalar)
        {
            return new Color(left.R * scalar, left.G * scalar, left.B * scalar, left.A * scalar);
        }

        public static explicit operator Vector4(Color color)
        {
            return new Vector4(color.R, color.G, color.B, color.A);
        }

        public static Color Lerp(Color a, Color b, float t)
        {
            return a + (b - a) * t;
        }

        public static Color Black   { get => new Color(0.0f, 0.0f, 0.0f, 1.0f); }
        public static Color White   { get => new Color(1.0f, 1.0f, 1.0f, 1.0f); }
        public static Color Red     { get => new Color(1.0f, 0.0f, 0.0f, 1.0f); }
        public static Color Green   { get => new Color(0.0f, 1.0f, 0.0f, 1.0f); }
        public static Color Blue    { get => new Color(0.0f, 0.0f, 1.0f, 1.0f); }
        public static Color Yellow  { get => new Color(1.0f, 1.0f, 0.0f, 1.0f); }
    }
}
