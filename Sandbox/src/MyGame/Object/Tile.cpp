#include "Tile.h"

Tile::Tile(const glm::vec2& size, const TileType& type)
{
	m_Size = size;
	m_Type = type;

	m_Texture = Texture2D::Create("assets/textures/spritesheet_ground.png");
	switch (type)
	{
		case TileType::GroundTile:
			m_TexCoords[0] = { 128.0f		   , 0.0f		   };
			m_TexCoords[1] = { 128.0f + 128.0f , 0.0f		   };
			m_TexCoords[2] = { 128.0f + 128.0f , 0.0f + 128.0f };
			m_TexCoords[3] = { 128.0f		   , 0.0f + 128.0f };
			break;

		case TileType::SurfaceTile:
			m_TexCoords[0] = { 0.0f		     , 128.0f };
			m_TexCoords[1] = { 0.0f + 128.0f , 128.0f };
			m_TexCoords[2] = { 0.0f + 128.0f , 128.0f + 128.0f };
			m_TexCoords[3] = { 0.0f		     , 128.0f + 128.0f };
			break;

		case TileType::FloatingTile:
			m_TexCoords[0] = { 128.0f		   , 128.0f };
			m_TexCoords[1] = { 128.0f + 128.0f , 128.0f };
			m_TexCoords[2] = { 128.0f + 128.0f , 128.0f + 128.0f };
			m_TexCoords[3] = { 128.0f		   , 128.0f + 128.0f };
			break;

		case TileType::FloatingLeftTile:
			m_TexCoords[0] = { 0.0f		     , 256.0f };
			m_TexCoords[1] = { 0.0f + 128.0f , 256.0f };
			m_TexCoords[2] = { 0.0f + 128.0f , 256.0f + 128.0f };
			m_TexCoords[3] = { 0.0f		     , 256.0f + 128.0f };
			break;

		case TileType::FloatingRightTile:
			m_TexCoords[0] = { 0.0f		     , 0.0f };
			m_TexCoords[1] = { 0.0f + 128.0f , 0.0f };
			m_TexCoords[2] = { 0.0f + 128.0f , 0.0f + 128.0f };
			m_TexCoords[3] = { 0.0f		     , 0.0f + 128.0f };
			break;
	}
}

void Tile::OnRender(const glm::vec2& position)
{
	Renderer2D::DrawQuad(position, m_Size, 0.0f, m_Texture, m_TexCoords);
}
