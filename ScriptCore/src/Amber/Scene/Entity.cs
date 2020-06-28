using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Amber
{
    public class Entity
    {
        public uint EntityID { get; private set; }
        public uint SceneID { get; private set; }

        ~Entity()
        {
            Console.WriteLine("Destroyed Entity {0}:{1}", SceneID, EntityID);
        }

        public T CreateComponent<T>() where T : Component, new()
        {
            CreateComponent_Native(SceneID, EntityID, typeof(T));
            T component = new T();
            component.Entity = this;
            return component;
        }

        public bool HasComponent<T>() where T : Component
        {
            return HasComponent_Native(SceneID, EntityID, typeof(T));
        }

        public T GetComponent<T>() where T : Component, new()
        {
            if (HasComponent<T>())
            {
                T component = new T();
                component.Entity = this;
                return component;
            }
            return null;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void CreateComponent_Native(uint sceneID, uint entityID, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool HasComponent_Native(uint sceneID, uint entityID, Type type);
    }
}
