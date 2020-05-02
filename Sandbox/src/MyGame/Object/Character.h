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

enum class CharacterType : uint32_t
{
	Player = (uint32_t)GameObjectType::Character | BIT(8),
	Enemy  = (uint32_t)GameObjectType::Character | BIT(9)
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
	virtual void SetState(const CharacterState& state) { m_State = state; }

	virtual const CharacterType& GetType() const { return m_Type; }
	virtual void SetType(const CharacterType& type) { m_Type = type; }

	virtual bool IsEnemy() const { return m_Type == CharacterType::Enemy; }

protected:
	glm::vec2 m_Position;
	AABB m_AABB;
	glm::vec2 m_AABBOffset;
	CharacterType m_Type;
	CharacterState m_State;

	Character() = default;
};
