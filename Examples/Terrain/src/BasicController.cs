using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Amber;
using Amber.Editor;

namespace Terrain
{
    class BasicController : Entity
    {
        [EditorField(Name = "Translation Speed")]
        public float translationSpeed = 50.0f;

        [EditorField(Name = "Rotation Speed")]
        public float rotationSpeed = 10.0f;

        public void OnUpdate(float ts)
        {
            TransformComponent component = GetComponent<TransformComponent>();
            Vector3 translation = new Vector3(0.0f);
            float rotation = 0.0f;

            float speed = translationSpeed * ts;
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

            speed = rotationSpeed * ts;
            if (Input.IsKeyPressed(KeyCode.Q))
                rotation -= speed;
            else if (Input.IsKeyPressed(KeyCode.E))
                rotation += speed;

            Matrix4 transform = component.Transform;
            transform = Matrix4.Translate(translation) * Matrix4.Rotate(rotation, new Vector3(0.0f, 1.0f, 0.0f)) * transform;
            component.Transform = transform;
        }
    }
}
