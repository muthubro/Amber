#include "Tile.h"

Tile::Tile(const glm::vec2& size, const GameObjectType& type)
{
	m_Size = size;
	m_Type = type;

	switch (type)
	{
		case GameObjectType::GroundTile:
			m_Texture = Texture2D::Create("assets/textures/BrownTile.png");
			break;

		case GameObjectType::SurfaceTile:
			m_Texture = Texture2D::Create("assets/textures/BrownTileTop.png");
			break;
	}
}

void Tile::OnRender(const glm::vec2& position)
{
	Renderer2D::DrawQuad(position, m_Size, 0.0f, m_Texture, 1.0f);
}
