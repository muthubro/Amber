using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Amber;

namespace Terrain
{
    class CameraController : Entity
    {
        private Entity PlayerEntity;

        void OnCreate()
        {
            PlayerEntity = FindEntitiesByTag("Player")[0];
        }

        void OnUpdate(float ts)
        {
            Matrix4 transform = GetTransform();
            Vector3 translation = transform.Translation;

            translation.XY = PlayerEntity.GetTransform().Translation.XY + new Vector2(0.0f, 0.5f);
            transform.Translation = translation;
            SetTransform(transform);
        }
    }
}
