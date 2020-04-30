#include "Level.h"

#include <glm/glm.hpp>

Level::Level(const std::string& mapString, uint32_t levelWidth, uint32_t levelHeight)
    : m_Width(levelWidth), m_Height(levelHeight), m_Player(glm::vec2(m_TileSize), this)
{
    HZ_ASSERT(mapString.size() == levelWidth * levelHeight, "Map dimensions are incorrect!");

    for (uint32_t i = 0; i < (uint32_t)GameObjectType::Count; i++) m_Tiles[i] = nullptr;
    BuildMap(mapString);
}

void Level::OnEvent(Event& e)
{
    m_Player.OnEvent(e);
}

void Level::OnUpdate(Timestep ts)
{
    m_Player.OnUpdate(ts);
}

void Level::OnRender(float left, float right, float bottom, float top)
{
    uint32_t leftCol = (uint32_t)std::max(0, (int)std::floor(left / m_TileSize));
    uint32_t rightCol = std::min(m_Width, (uint32_t)std::ceil(right / m_TileSize) + 1);
    uint32_t bottomRow = (uint32_t)std::max(0, (int)std::floor(bottom / m_TileSize));
    uint32_t topRow = std::min(m_Height, (uint32_t)std::ceil(top / m_TileSize) + 1);

    for (uint32_t row = bottomRow; row < topRow; row++)
    {
        for (uint32_t col = leftCol; col < rightCol; col++)
        {
            glm::mat4 color = {
                { 0.0f, 0.0f, 0.0f, 1.0f},
                { 0.0f, 0.0f, 0.0f, 1.0f},
                { 1.0f, 1.0f, 1.0f, 1.0f},
                { 1.0f, 1.0f, 1.0f, 1.0f}
            };
            if (row == 14)
            {
                //Renderer2D::DrawQuad({ col * m_TileSize, row * m_TileSize }, { m_TileSize, m_TileSize }, 0.0f, color);
                //m_Tiles[m_Map[row-1][col]]->OnRender({ col * m_TileSize, row * m_TileSize });

                continue;
            }
            else if (row == 13)
            {
                //Renderer2D::DrawQuad({ col * m_TileSize, row * m_TileSize }, { m_TileSize, m_TileSize }, 0.0f, { 1.0f, 0.0f, 0.0f, 1.0f });
                //m_Tiles[m_Map[row+1][col]]->OnRender({ col * m_TileSize, row * m_TileSize });

                continue;
            }
            if (m_Map[row][col])
                m_Tiles[m_Map[row][col]]->OnRender(TileToPosition(row, col));
        }
    }

    m_Player.OnRender();
}

void Level::BuildMap(const std::string& mapString)
{
    for (uint32_t row = 0; row < m_Height; row++)
    {
        uint32_t flippedRow = m_Height - row - 1;
        for (uint32_t col = 0; col < m_Width; col++)
        {
            uint8_t obj = mapString[row * m_Width + col];

            switch (obj)
            {
                case '1':
                    m_Map[flippedRow][col] = 1;
                    if (!m_Tiles[1])
                        m_Tiles[1] = CreateRef<Tile>(glm::vec2(m_TileSize), GameObjectType::GroundTile);
                    break;

                case '2':
                    m_Map[flippedRow][col] = 2;
                    if (!m_Tiles[2])
                        m_Tiles[2] = CreateRef<Tile>(glm::vec2(m_TileSize), GameObjectType::SurfaceTile);
                    break;

                case 'P':
                    m_Player.SetPosition({ col * m_TileSize, flippedRow * m_TileSize });
                    m_Map[flippedRow][col] = 0;
                    break;

                default:
                    HZ_ASSERT(obj == ' ', "Unknown object type in map!");
                    m_Map[flippedRow][col] = 0;
            }
        }
    }
}
