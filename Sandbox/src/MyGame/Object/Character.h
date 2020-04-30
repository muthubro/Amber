#pragma once

#include <glm/glm.hpp>

#include <Hazel.h>

#include "GameObject.h"

using namespace Hazel;

struct AABB
{
	glm::vec2 Center;
	glm::vec2 HalfSize;

	AABB() = default;
	AABB(const glm::vec2& center, const glm::vec2& halfSize)
		: Center(center), HalfSize(halfSize) {}

	bool Overlaps(const AABB& other)
	{
		if (std::abs(other.Center.x - Center.x) > other.HalfSize.x + HalfSize.x) return false;
		if (std::abs(other.Center.y - Center.y) > other.HalfSize.y + HalfSize.y) return false;
		return true;
	}
};

enum class CharacterState
{
	Idle = 0,
	Standing,
	Walking, Jumping
};

class Character : public GameObject
{
public:
	virtual ~Character() = default;

	virtual const glm::vec2& GetPosition() const { return m_Position; }
	virtual void SetPosition(const glm::vec2& position) { m_Position = position; m_AABB.Center = position + m_AABBOffset; }

	virtual const CharacterState& GetState() const { return m_State; }
	virtual void SetPosition(const CharacterState& state) { m_State = state; }

protected:
	glm::vec2 m_Position;
	AABB m_AABB;
	glm::vec2 m_AABBOffset;
	CharacterState m_State;
	Ref<Texture2D> m_Texture;

	Character() = default;
};
