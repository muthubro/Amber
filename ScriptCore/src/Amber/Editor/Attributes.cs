using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Amber.Editor
{
    [AttributeUsage(
        AttributeTargets.Field |
        AttributeTargets.Property,
        AllowMultiple = false)]
    public class EditorField : System.Attribute
    {
        public string Name;
        public float Min = 0.0f;
        public float Max = 100.0f;
        public float Step = 1.0f;

        public EditorField()
        { 
        }

        public EditorField(string name, float min, float max, float step)
        {
            Name = name;
            Min = min;
            Max = max;
            Step = step;
        }
    }
}
