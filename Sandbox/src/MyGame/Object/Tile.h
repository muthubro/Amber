#pragma once

#include <Hazel.h>

#include "GameObject.h"

using namespace Hazel;

class Tile : public GameObject
{
public:
	Tile(const glm::vec2& size, const GameObjectType& type);
	virtual ~Tile() = default;

	void OnRender(const glm::vec2& position);

protected:
	Ref<Texture2D> m_Texture;
};
