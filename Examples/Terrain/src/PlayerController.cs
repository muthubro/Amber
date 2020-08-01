using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Amber;

namespace Terrain
{
    class PlayerController : Entity
    {
        public float HorizontalForce = 10.0f;
        public float VerticalForce = 10.0f;

        private RigidBody2DComponent PhysicsBody;

        void OnCreate()
        {
            PhysicsBody = GetComponent<RigidBody2DComponent>();
        }

        void OnUpdate(float ts)
        {
            if (Input.IsKeyPressed(KeyCode.D))
                PhysicsBody.ApplyLinearImpulse(new Vector2(HorizontalForce, 0.0f), new Vector2(), true);
            else if (Input.IsKeyPressed(KeyCode.A))
                PhysicsBody.ApplyLinearImpulse(new Vector2(-HorizontalForce, 0.0f), new Vector2(), true);

            if (Input.IsKeyPressed(KeyCode.Space))
                PhysicsBody.ApplyLinearImpulse(new Vector2(0, VerticalForce), new Vector2(), true);
        }
    }
}
