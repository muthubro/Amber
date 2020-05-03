#pragma once

#include <glm/glm.hpp>

#include <Amber.h>

#include "Object/Player.h"
#include "Object/Tile.h"

#define MAX_LEVEL_SIZE 100

class Level
{
public:
	Level(const std::string& mapString, uint32_t levelWidth, uint32_t levelHeight);

	void OnEvent(Event& e);
	void OnRender(float left, float right, float bottom, float top);
	void OnUpdate(Timestep ts);

	float GetTileSize() const { return m_TileSize; }
    uint32_t GetWidth() const { return m_Width; }
    uint32_t GetHeight() const { return m_Height; }

	const glm::vec2& GetPlayerPosition() const { return m_Player.GetPosition(); }
    Ref<Tile> GetTile(uint32_t row, uint32_t col) const { return m_Map[row][col] ? m_Tiles[m_Map[row][col]] : nullptr; }

    std::pair<uint32_t, uint32_t> PositionToTile(const glm::vec2& pos) const { return { YToRow(pos.y), XToColumn(pos.x) }; }
    uint32_t XToColumn(float x) const { return std::min(m_Width - 1, (uint32_t)std::round(x / m_TileSize)); }
    uint32_t YToRow(float y) const { return std::min(m_Height - 1, (uint32_t)std::round(y / m_TileSize)); }

    glm::vec2 TileToPosition(uint32_t row, uint32_t column) const { return { ColumnToX(column), RowToY(row) }; }
    float ColumnToX(uint32_t column) const { return (float)std::min(m_Width - 1, column) * m_TileSize; }
    float RowToY(uint32_t row) const { return (float)std::min(m_Height - 1, row) * m_TileSize; }

private:
	uint32_t m_Width, m_Height;
	float m_TileSize = 32.0f;

	Ref<Texture2D> m_CloudTexture;
	Ref<Texture2D> m_SunTexture;

	uint8_t m_Map[MAX_LEVEL_SIZE][MAX_LEVEL_SIZE];
	Ref<Tile> m_Tiles[(uint32_t)TileType::Count];

	Player m_Player;

	void BuildMap(const std::string& mapString);
};