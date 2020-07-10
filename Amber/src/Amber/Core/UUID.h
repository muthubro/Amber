#pragma once

#include <xhash>

#include "Amber/Math/Random.h"

namespace Amber
{

class UUID
{
public:
    UUID() : m_UUID(Math::Random::UInt()) {}
    UUID(uint64_t uuid) : m_UUID(uuid) {}
    UUID(const UUID& other) : m_UUID(other.m_UUID) {}

    operator uint64_t() { return m_UUID; }
    operator uint64_t() const { return m_UUID; }

private:
    uint64_t m_UUID;
};

}

namespace std
{

template<>
struct hash<Amber::UUID>
{
    size_t operator()(const Amber::UUID& uuid) const
    {
        return hash<uint64_t>()(uuid);
    }
};

}