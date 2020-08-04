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
        public Vector2 MaxSpeed = new Vector2(1.0f, 1.0f);
        public float Friction = 0.2f;
        public float Damping = 0.2f;
        public float TriggerTimeout = 5.0f;

        private RigidBody2DComponent PhysicsBody;
        private Mesh PlayerMesh;
        private Entity Gate;

        private int CollisionCount = 0;
        private bool IsColliding => CollisionCount > 0;

        private bool GateOpened = false;
        private bool PressingTrigger = false;
        private float TriggerTime = 0.0f;

        void OnCreate()
        {
            PhysicsBody = GetComponent<RigidBody2DComponent>();

            PlayerMesh = GetComponent<MeshComponent>().Mesh;
            PlayerMesh.SetAlbedo(Color.White);

            Gate = FindEntitiesByTag("Gate")[0];

            FindEntitiesByTag("Ground")[0].GetComponent<MeshComponent>().Mesh.SetAlbedo(Color.Green);
            FindEntitiesByTag("Wall").ForEach(entity => entity.GetComponent<MeshComponent>().Mesh.SetAlbedo(new Color(64.0f / 255.0f, 25.0f / 255.0f, 0.0f)));
            FindEntitiesByTag("Platform").ForEach(entity => entity.GetComponent<MeshComponent>().Mesh.SetAlbedo(new Color(25.0f / 255.0f, 5.0f / 255.0f, 0.0f)));
            FindEntitiesByTag("Trigger").ForEach(entity => entity.GetComponent<MeshComponent>().Mesh.SetAlbedo(Color.Red));

            AddCollision2DBeginCallback(OnCollisionEnter);
            AddCollision2DEndCallback(OnCollisionExit);
        }

        void OnCollisionEnter(Entity other)
        {
            CollisionCount++;
            PlayerMesh.SetAlbedo(Color.Red);

            string tag = other.GetComponent<TagComponent>().Tag;
            if (tag == "Trigger")
            {
                if (!GateOpened)
                {
                    other.GetComponent<MeshComponent>().Mesh.SetAlbedo(Color.Green);
                    GateOpened = true;
                }

                PressingTrigger = true;
                TriggerTime = 0.0f;
            }
            else if (tag == "Gate")
            {
                if (GateOpened)
                {
                    other.GetComponent<MeshComponent>().Mesh.SetAlbedo(Color.Green);
                    Console.WriteLine("You Win!");
                }
            }
        }

        void OnCollisionExit(Entity other)
        {
            CollisionCount--;
            if (!IsColliding)
                PlayerMesh.SetAlbedo(Color.White);

            if (other.GetComponent<TagComponent>().Tag == "Trigger")
                PressingTrigger = false;
        }

        void MovePlayer(float ts)
        {
            float movementForce = HorizontalForce;
            if (!IsColliding)
                movementForce *= (1.0f - Damping);

            if (Input.IsKeyPressed(KeyCode.D))
                PhysicsBody.ApplyLinearImpulse(new Vector2(movementForce, 0.0f), new Vector2(), true);
            else if (Input.IsKeyPressed(KeyCode.A))
                PhysicsBody.ApplyLinearImpulse(new Vector2(-movementForce, 0.0f), new Vector2(), true);

            if (IsColliding && Input.IsKeyPressed(KeyCode.Space))
                PhysicsBody.ApplyLinearImpulse(new Vector2(0, VerticalForce), new Vector2(), true);

            Vector2 linearVelocity = PhysicsBody.GetLinearVelocity();
            if (IsColliding)
                linearVelocity.X *= (1.0f - Friction * 9.81f * ts);
            linearVelocity = Vector2.Clamp(linearVelocity, new Vector2(-MaxSpeed.X, -10.0f), MaxSpeed);
            PhysicsBody.SetLinearVelocity(linearVelocity);
        }

        void OnUpdate(float ts)
        {
            MovePlayer(ts);

            if (!PressingTrigger && GateOpened)
            {
                TriggerTime += ts;
                if (TriggerTime >= TriggerTimeout)
                {
                    GateOpened = false;
                    FindEntitiesByTag("Trigger")[0].GetComponent<MeshComponent>().Mesh.SetAlbedo(Color.Red);
                }    
            }
        }
    }
}
