using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Amber;

namespace Terrain
{
    class BasicController : Entity
    {
        public float Speed = 50.0f;

        public void OnUpdate(float ts)
        {
            TransformComponent component = GetComponent<TransformComponent>();
            Vector3 translation = new Vector3(0.0f);

            float speed = Speed * ts;

            if (Input.IsKeyPressed(KeyCode.D))
                translation.X += speed;
            else if (Input.IsKeyPressed(KeyCode.A))
                translation.X -= speed;
            if (Input.IsKeyPressed(KeyCode.W))
                translation.Y += speed;
            else if (Input.IsKeyPressed(KeyCode.S))
                translation.Y -= speed;
            if (Input.IsKeyPressed(KeyCode.Up))
                translation.Z -= speed;
            else if (Input.IsKeyPressed(KeyCode.Down))
                translation.Z += speed;

            Matrix4 transform = component.Transform;
            transform[3] += new Vector4(translation.X, translation.Y, translation.Z, 0.0f);
            component.Transform = transform;
        }
    }
}
