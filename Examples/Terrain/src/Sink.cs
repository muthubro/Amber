using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Amber;

namespace Terrain
{
    class Sink : Entity
    {
        public float Gravity = 1.0f;
        private float speed = 0.0f;

        void OnUpdate(float ts)
        {
            speed -= Gravity * ts;

            TransformComponent transformComponent = GetComponent<TransformComponent>();
            Matrix4 transform = transformComponent.Transform;
            transformComponent.Transform = Matrix4.Translate(new Vector3(0.0f, speed * ts, 0.0f)) * transform;
        }
    }
}
