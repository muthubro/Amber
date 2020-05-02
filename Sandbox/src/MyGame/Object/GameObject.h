#pragma once

#include <glm/glm.hpp>

// First byte is reserved for GameObjectType
enum class GameObjectType : uint32_t
{
	Empty		= 0,
	Block		= BIT(1),
	Character   = BIT(2),
};

class GameObject
{
public:
	virtual ~GameObject() = default;

	virtual const glm::vec2& GetSize() const { return m_Size; }
	virtual void SetSize(const glm::vec2& size) { m_Size = size; }

protected:
	glm::vec2 m_Size;

	GameObject() = default;
};
