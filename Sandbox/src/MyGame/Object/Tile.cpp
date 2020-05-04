#include "Tile.h"

Tile::Tile(const glm::vec2& size, const TileType& type)
{
	m_Size = size;
	m_Type = type;

	switch (type)
	{
		case TileType::GroundTile:
			m_Texture = Texture2D::Create("assets/textures/grassCenter.png");
			break;

		case TileType::SurfaceTile:
			m_Texture = Texture2D::Create("assets/textures/grassMid.png");
			break;

		case TileType::FloatingTile:
			m_Texture = Texture2D::Create("assets/textures/grass.png");
			break;

		case TileType::FloatingLeftTile:
			m_Texture = Texture2D::Create("assets/textures/grassLeft.png");
			break;

		case TileType::FloatingRightTile:
			m_Texture = Texture2D::Create("assets/textures/grassRight.png");
			break;
	}
}

void Tile::OnRender(const glm::vec2& position, float shade)
{
	Renderer2D::DrawQuad(position, m_Size, 0.0f, m_Texture, { shade, shade, shade, 1.0f });
}
