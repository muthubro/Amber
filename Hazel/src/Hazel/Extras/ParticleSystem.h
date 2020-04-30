#pragma once

#include <glm/glm.hpp>

#include "Hazel/Core/Timestep.h"

#include "Hazel/Renderer/Texture.h"

namespace Hazel
{

struct ParticleProps
{
	glm::vec2 Position;
	glm::vec2 Velocity, VelocityVariation;
	glm::vec4 ColorBegin, ColorEnd;
	float SizeBegin, SizeEnd, SizeVariation;
	float Lifetime;
	float Gravity = 0.0f;
	bool DoesRotate = false;
};

enum class ParticleType
{
	Square, Circle
};

class ParticleSystem
{
public:
	ParticleSystem(const uint32_t poolSize, ParticleType type = ParticleType::Square);

	void Emit(const ParticleProps& properties, uint32_t particleCount);

	void OnUpdate(Timestep ts);

private:
	struct Particle
	{
		glm::vec2 Position;
		float Rotation;
		glm::vec2 Velocity;
		glm::vec4 ColorBegin, ColorEnd;
		float SizeBegin, SizeEnd;
		float Lifetime, LifeRemaining;

		bool Active = false;
		bool DoesRotate = false;
		float Gravity = 0.0f;
	};

	const uint32_t m_PoolSize;
	uint32_t m_PoolIndex;
	std::vector<Particle> m_ParticlePool;
	ParticleType m_ParticleType;
	Ref<Texture2D> m_ParticleTexture;
};

}
