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
        public ulong ID { get; private set; }

        private List<Action<Entity>> Collision2DBeginCallbacks = new List<Action<Entity>>();
        private List<Action<Entity>> Collision2DEndCallbacks = new List<Action<Entity>>();

        ~Entity()
        {
        }

        protected Entity() { }

        internal Entity(ulong id)
        {
            ID = id;
        }

        public Matrix4 GetTransform()
        {
            return GetComponent<TransformComponent>().Transform;
        }

        public void SetTransform(Matrix4 transform)
        {
            GetComponent<TransformComponent>().Transform = transform;
        }

        public T CreateComponent<T>() where T : Component, new()
        {
            CreateComponent_Native(ID, typeof(T));
            T component = new T();
            component.Entity = this;
            return component;
        }

        public bool HasComponent<T>() where T : Component
        {
            return HasComponent_Native(ID, typeof(T));
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

        public List<Entity> FindEntitiesByTag(string tag)
        {
            FindEntitiesByTag_Native(tag, out ulong[] entities);

            List<Entity> result = new List<Entity>();
            foreach (var entity in entities)
                result.Add(new Entity(entity));

            return result;
        }

        public void AddCollision2DBeginCallback(Action<Entity> callback)
        {
            Collision2DBeginCallbacks.Add(callback);
        }

        public void AddCollision2DEndCallback(Action<Entity> callback)
        {
            Collision2DEndCallbacks.Add(callback);
        }

        private void OnCollision2DBegin(ulong ID)
        {
            foreach (var callback in Collision2DBeginCallbacks)
                callback.Invoke(new Entity(ID));
        }

        private void OnCollision2DEnd(ulong ID)
        {
            foreach (var callback in Collision2DEndCallbacks)
                callback.Invoke(new Entity(ID));
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void CreateComponent_Native(ulong ID, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool HasComponent_Native(ulong ID, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void FindEntitiesByTag_Native(string tag, out ulong[] outArray);
    }
}
