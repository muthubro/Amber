#pragma once

#include <glm/glm.hpp>

enum class GameObjectType
{
	Empty = 0,
	GroundTile, SurfaceTile,
	Player,
	Enemy,
	Count
};

class GameObject
{
public:
	virtual ~GameObject() = default;

	virtual const glm::vec2& GetSize() const { return m_Size; }
	virtual void SetSize(const glm::vec2& size) { m_Size = size; }

	virtual const GameObjectType& GetType() const { return m_Type; }
	virtual void SetType(const GameObjectType& type) { m_Type = type; }

	virtual bool IsGround() const { return m_Type == GameObjectType::GroundTile || m_Type == GameObjectType::SurfaceTile; }
	virtual bool IsEnemy() const { return m_Type == GameObjectType::Enemy; }

protected:
	glm::vec2 m_Size;
	GameObjectType m_Type;

	GameObject() = default;
};
