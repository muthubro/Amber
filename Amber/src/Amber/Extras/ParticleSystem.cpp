#include "abpch.h"
#include "ParticleSystem.h"

#include <glm/gtx/compatibility.hpp>

#include "Amber/Renderer/Renderer2D.h"

#include "Amber/Utils/Random.h"

namespace Amber
{

ParticleSystem::ParticleSystem(const uint32_t poolSize, ParticleType type)
    : m_PoolSize(poolSize), m_ParticleType(type)
{
    m_ParticlePool.resize(poolSize);

    switch (type)
    {
        case ParticleType::Square:
            m_ParticleTexture = nullptr;
            m_PoolIndex = poolSize - 1;
            break;

        case ParticleType::Circle:
            m_ParticleTexture = Texture2D::Create("assets/textures/Particle.png");
            m_PoolIndex = 0;
            break;
    }
}

void ParticleSystem::Emit(const ParticleProps& properties, uint32_t particleCount)
{
    for (uint32_t i = 0; i < particleCount; i++)
    {
        Particle& particle = m_ParticlePool[m_PoolIndex];
        particle.Active = true;

        particle.Position = properties.Position;
        particle.Velocity = properties.Velocity;
        particle.Velocity.x += properties.VelocityVariation.x * (Random::Float() - 0.5f);
        particle.Velocity.y += properties.VelocityVariation.y * (Random::Float() - 0.5f);
        
        particle.ColorBegin = properties.ColorBegin;
        particle.ColorEnd = properties.ColorEnd;

        particle.SizeBegin = properties.SizeBegin + properties.SizeVariation * (Random::Float() - 0.5f);
        particle.SizeEnd = properties.SizeEnd;

        particle.Lifetime = properties.Lifetime;
        particle.LifeRemaining = properties.Lifetime;

        particle.Gravity = properties.Gravity;

        particle.DoesRotate = m_ParticleType == ParticleType::Square ? properties.DoesRotate : false;
        if (particle.DoesRotate)
            particle.Rotation = (Random::Float() - 0.5f) * 45.0f;

        switch (m_ParticleType)
        {
            case ParticleType::Square:
                m_PoolIndex = m_PoolIndex ? m_PoolIndex - 1 : m_PoolSize - 1;
                break;

            case ParticleType::Circle:
                m_PoolIndex = (m_PoolIndex + 1) % m_PoolSize;
                break;
        }
    }
}


void ParticleSystem::OnUpdate(Timestep ts)
{
    uint32_t idx;
    switch (m_ParticleType)
    {
        case ParticleType::Square:
            idx = 0;
            break;

        case ParticleType::Circle:
            idx = (m_PoolIndex + 1) % m_PoolSize;
            break;
    }
    
    for (uint32_t i = 0; i < m_PoolSize; i++, idx = (idx + 1) % m_PoolSize)
    {
        auto& particle = m_ParticlePool[idx];

        if (!particle.Active) continue;

        if (particle.LifeRemaining <= 0.0f)
        {
            particle.Active = false;
            continue;
        }

        particle.LifeRemaining -= ts;
        particle.Position += particle.Velocity * (float)ts;
        particle.Velocity.y -= particle.Gravity * (float)ts;
        if (particle.DoesRotate)
            particle.Rotation += glm::radians(180.0f * ts);

        float life = particle.LifeRemaining / particle.Lifetime;
        float size = glm::lerp(particle.SizeEnd, particle.SizeBegin, life);
        glm::vec4 color = glm::lerp(particle.ColorEnd, particle.ColorBegin, life);

        Renderer2D::QuadData data(particle.Position, { size, size }, particle.Rotation, m_ParticleTexture);
        data.SetColor(color);
        Renderer2D::DrawQuad(data);
    }
}

}