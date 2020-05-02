#pragma once

#include <glm/glm.hpp>

#include <Amber.h>

#include "Object/Player.h"
#include "Object/Tile.h"

#define MAX_LEVEL_SIZE 100


static glm::vec2 WillCollide(glm::vec2 rect1[4], glm::vec2 rect2[4], glm::vec2 velocity)
{
    glm::vec2 minDeflectionAxis = { 0.0f, 0.0f };
    float minDeflection = std::numeric_limits<float>::max();

    for (uint8_t i = 0; i < 4; i++)
    {
        glm::vec2& p1 = rect1[i];
        glm::vec2& p2 = rect1[(i + 1) % 4];

        glm::vec2 normal = { p2.y - p1.y, p1.x - p2.x };
        float mod = sqrt(normal.x * normal.x + normal.y * normal.y);
        normal /= mod;

        float velocityProjection = normal.x * velocity.x + normal.y * velocity.y;

        float minA = std::numeric_limits<float>::max();
        float maxA = -(std::numeric_limits<float>::max() - 1.0f);
        for (uint8_t j = 0; j < 4; j++)
        {
            float projected = normal.x * rect1[j].x + normal.y * rect1[j].y;
            if (projected < minA)
                minA = projected;
            if (projected > maxA)
                maxA = projected;
        }

        float minB = std::numeric_limits<float>::max();
        float maxB = -(std::numeric_limits<float>::max() - 1.0f);
        for (uint8_t j = 0; j < 4; j++)
        {
            float projected = normal.x * rect2[j].x + normal.y * rect2[j].y;
            if (projected < minB)
                minB = projected;
            if (projected > maxB)
                maxB = projected;
        }

        if (velocityProjection < 0)
            minA += velocityProjection;
        else
            maxA += velocityProjection;

        if (maxA < minB || maxB < minA)
            return { 0.0f, 0.0f };

        float distance = minA < minB ? minB - maxA : minA - maxB;
        distance = std::abs(distance);
        if (distance < minDeflection)
        {
            minDeflection = distance;
            minDeflectionAxis = normal;

            glm::vec2 centerA = { 0.0f, 0.0f };
            for (uint8_t i = 0; i < 4; i++)
                centerA += rect1[i];
            centerA *= 0.25f;

            glm::vec2 centerB = { 0.0f, 0.0f };
            for (uint8_t i = 0; i < 4; i++)
                centerB += rect2[i];
            centerB *= 0.25f;

            glm::vec2 dist = centerA - centerB;
            if (normal.x * dist.x + normal.y * dist.y < 0)
                minDeflectionAxis = -minDeflectionAxis;
        }
    }

    for (uint8_t i = 0; i < 4; i++)
    {
        glm::vec2& p1 = rect2[i];
        glm::vec2& p2 = rect2[(i + 1) % 4];

        glm::vec2 normal = { p2.y - p1.y, p1.x - p2.x };

        float velocityProjection = normal.x * velocity.x + normal.y * velocity.y;

        float minA = std::numeric_limits<float>::max();
        float maxA = -(std::numeric_limits<float>::max() - 1.0f);
        for (uint8_t j = 0; j < 4; j++)
        {
            float projected = normal.x * rect1[j].x + normal.y * rect1[j].y;
            if (projected < minA)
                minA = projected;
            if (projected > maxA)
                maxA = projected;
        }

        float minB = std::numeric_limits<float>::max();
        float maxB = -(std::numeric_limits<float>::max() - 1.0f);
        for (uint8_t j = 0; j < 4; j++)
        {
            float projected = normal.x * rect2[j].x + normal.y * rect2[j].y;
            if (projected < minB)
                minB = projected;
            if (projected > maxB)
                maxB = projected;
        }

        if (velocityProjection < 0)
            minA += velocityProjection;
        else
            maxA += velocityProjection;

        if (maxA < minB || maxB < minA)
            return { 0.0f, 0.0f };

        float distance = minA < minB ? minB - maxA : minA - maxB;
        distance = std::abs(distance);
        if (distance < minDeflection)
        {
            minDeflection = distance;
            minDeflectionAxis = normal;

            glm::vec2 centerA = { 0.0f, 0.0f };
            for (uint8_t i = 0; i < 4; i++)
                centerA += rect1[i];
            centerA *= 0.25f;

            glm::vec2 centerB = { 0.0f, 0.0f };
            for (uint8_t i = 0; i < 4; i++)
                centerB += rect2[i];
            centerB *= 0.25f;

            glm::vec2 dist = centerA - centerB;
            if (normal.x * dist.x + normal.y * dist.y < 0)
                minDeflectionAxis = -minDeflectionAxis;
        }
    }

    return minDeflectionAxis * minDeflection;
}

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

	uint8_t m_Map[MAX_LEVEL_SIZE][MAX_LEVEL_SIZE];
	Ref<Tile> m_Tiles[(uint32_t)TileType::Count];

	Player m_Player;

	void BuildMap(const std::string& mapString);
};