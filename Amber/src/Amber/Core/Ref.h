#pragma once

#include <stdint.h>

namespace Amber
{

class RefCounted
{
public:
    void IncRefCount() const { m_RefCount++; }
    void DecRefCount() const { m_RefCount--; }

    uint32_t GetRefCount() const { return m_RefCount; }

private:
    mutable uint32_t m_RefCount = 0;
};

template<typename T>
class WeakRef;

template<typename T>
class Ref
{
public:
    Ref()
        : m_Instance(nullptr) 
    {
    }

    ~Ref()
    {
        DecRef();
    }

    Ref(std::nullptr_t null)
        : m_Instance(nullptr)
    {
    }

    Ref(T* other)
        : m_Instance(other)
    {
        static_assert(std::is_base_of<RefCounted, T>::value, "Class is not RefCounted!");

        IncRef();
    }

    Ref(const Ref<T>& other)
        : m_Instance(other.m_Instance)
    {
        IncRef();
    }

    Ref(Ref<T>&& other)
        : m_Instance(other.m_Instance)
    {
        other.m_Instance = nullptr;
    }

    template<typename T2>
    Ref(const Ref<T2>& other)
    {
        m_Instance = other.m_Instance;
        IncRef();
    }

    template<typename T2>
    Ref(Ref<T2>&& other)
    {
        m_Instance = other.m_Instance;
        other.m_Instance = nullptr;
    }

   Ref(const WeakRef<T>& other)
   {
       m_Instance = nullptr;
       if (other.m_Instance)
       {
           m_Instance = other.m_Instance;
           IncRef();
       }
   }
   
   template<typename T2>
   Ref(const WeakRef<T2>& other)
   {
       m_Instance = nullptr;
       if (other.m_Instance)
       {
           m_Instance = other.m_Instance;
           IncRef();
       }
   }

    Ref& operator=(std::nullptr_t null)
    {
        DecRef();
        m_Instance = nullptr;
        return *this;
    }

    Ref& operator=(const Ref<T>& other)
    {
        other.IncRef();
        DecRef();

        m_Instance = other.m_Instance;
        return *this;
    }

    template<typename T2>
    Ref& operator=(const Ref<T2>& other)
    {
        other.IncRef();
        DecRef();

        m_Instance = other.m_Instance;
        return *this;
    }

    template<typename T2>
    Ref& operator=(Ref<T2>&& other)
    {
        DecRef();

        m_Instance = other.m_Instance;
        other.m_Instance = nullptr;
        return *this;
    }

    operator bool() { return m_Instance != nullptr; }
    operator bool() const { return m_Instance != nullptr; }

    T* operator->() { return m_Instance; }
    const T* operator->() const { return m_Instance; }

    T& operator*() { return *m_Instance; }
    const T& operator*() const { return *m_Instance; }

    void Reset(T* instance = nullptr)
    {
        DecRef();
        m_Instance = instance;
    }

    template<typename... Args>
    static Ref<T> Create(Args&&... args)
    {
        return Ref<T>(new T(std::forward<Args>(args)...));
    }

private:
    T* m_Instance;

    void IncRef() const
    {
        if (m_Instance)
            m_Instance->IncRefCount();
    }

    void DecRef() const
    {
        if (m_Instance)
        {
            m_Instance->DecRefCount();
            if (m_Instance->GetRefCount() == 0)
                delete m_Instance;
        }
    }

    template<typename T2>
    friend class Ref;
    template<typename T2>
    friend class WeakRef;
};

template<typename T>
class WeakRef
{
public:
    WeakRef()
        : m_Instance(nullptr)
    {
    }

    WeakRef(std::nullptr_t null)
        : m_Instance(nullptr)
    {
    }

    WeakRef(const WeakRef<T>& other)
        : m_Instance(other.m_Instance)
    {
    }

    WeakRef(WeakRef<T>&& other)
        : m_Instance(other.m_Instance)
    {
        other.m_Instance = nullptr;
    }

    template<typename T2>
    WeakRef(const WeakRef<T2>& other)
    {
        m_Instance = other.m_Instance;
    }

    template<typename T2>
    WeakRef(WeakRef<T2>&& other)
    {
        m_Instance = other.m_Instance;
        other.m_Instance = nullptr;
    }

    WeakRef(const Ref<T>& other)
        : m_Instance(other.m_Instance)
    {
    }

    template<typename T2>
    WeakRef(const Ref<T2>& other)
    {
        m_Instance = other.m_Instance;
    }

    Ref<T> Get()
    {
        return Ref<T>(*this);
    }

private:
    T* m_Instance;

    template<typename T2>
    friend class Ref;
};

}
