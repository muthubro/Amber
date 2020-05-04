#include "Level.h"

#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>

#include "Color.h"

Level::Level(const std::string& mapString, uint32_t levelWidth, uint32_t levelHeight)
    : m_Width(levelWidth), m_Height(levelHeight), m_Player(glm::vec2(m_TileSize), this)
{
    AB_ASSERT(mapString.size() == levelWidth * levelHeight, "Map dimensions are incorrect!");

    for (uint32_t i = 0; i < (uint32_t)TileType::Count; i++) m_Tiles[i] = nullptr;
    BuildMap(mapString);

    m_CloudTexture = Texture2D::Create("assets/textures/Cloud.png");
    m_SunTexture = Texture2D::Create("assets/textures/Sun.png");
    m_MoonTexture = Texture2D::Create("assets/textures/Circle.png");
}

void Level::OnEvent(Event& e)
{
    m_Player.OnEvent(e);
}

void Level::OnUpdate(Timestep ts)
{
    m_Time += ts;
    if (m_Time >= m_DayLength) m_Time -= m_DayLength;

    m_Player.OnUpdate(ts);
}

void Level::OnRender(float left, float right, float bottom, float top)
{
    float timeRatio = 0.5f * (1.0f + std::cos(m_Time * 2.0 * std::_Pi / m_DayLength));

    glm::vec4 clearColor;
    if (timeRatio > 0.3f)
        clearColor = glm::lerp(Color::SunSet, Color::SkyBlue, (timeRatio - 0.3f) / 0.7f);
    else if (timeRatio > 0.1f)
        clearColor = glm::lerp(Color::MidnightBlue, Color::SunSet, (timeRatio - 0.1f) / 0.2f);
    else
        clearColor = Color::MidnightBlue;

    RenderCommand::SetClearColor(clearColor);
    RenderCommand::Clear();

    uint32_t leftCol = (uint32_t)std::max(0, (int)std::floor(left / m_TileSize));
    uint32_t rightCol = std::min(m_Width, (uint32_t)std::ceil(right / m_TileSize) + 1);
    uint32_t bottomRow = (uint32_t)std::max(0, (int)std::floor(bottom / m_TileSize));
    uint32_t topRow = std::min(m_Height, (uint32_t)std::ceil(top / m_TileSize) + 1);

    if (topRow >= 15)
    {
        float cloudAlpha = glm::lerp(0.0f, 1.0f, timeRatio);

        uint32_t col = leftCol > 2 ? leftCol - 2 : 0;
        for (; col < std::min(rightCol + 2, m_Width); col++)
        {
            if (col % 10 == 0)
            {
                auto pos = glm::vec2(ColumnToX(col), top - 160.0f) + glm::vec2(m_TileSize * 1.5f);
                Renderer2D::DrawQuad(pos, glm::vec2(4 * m_TileSize), 0.0f, m_CloudTexture, { 1.0f, 1.0f, 1.0f, cloudAlpha });
            }
        }
    }

    if (timeRatio > 0.3f)
    {
        float sunPos = glm::lerp(bottom - 100.0f, top - 160.0f, (timeRatio - 0.3f) / 0.7f);
        auto sunColor = glm::lerp(Color::SunSet, Color::Yellow, (timeRatio - 0.3f) / 0.7f);
        Renderer2D::DrawQuad(glm::vec2(left + 64.0f, sunPos) + glm::vec2(m_TileSize), glm::vec2(3 * m_TileSize), 0.0f, m_SunTexture, sunColor);
    }
    else
    {
        float moonPos = glm::lerp(top - 160.0f, bottom - 100.0f, timeRatio / 0.3f);
        Renderer2D::DrawQuad(glm::vec2(left + 64.0f, moonPos) + glm::vec2(m_TileSize), glm::vec2(3 * m_TileSize), 0.0f, m_MoonTexture);
    }

    float shade = glm::lerp(0.15f, 1.0f, timeRatio);
    for (uint32_t row = bottomRow; row < topRow; row++)
    {
        for (uint32_t col = leftCol; col < rightCol; col++)
        {
            if (m_Map[row][col])
                m_Tiles[m_Map[row][col]]->OnRender(TileToPosition(row, col), shade);
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
                        m_Tiles[1] = CreateRef<Tile>(glm::vec2(m_TileSize), TileType::GroundTile);
                    break;

                case '2':
                    m_Map[flippedRow][col] = 2;
                    if (!m_Tiles[2])
                        m_Tiles[2] = CreateRef<Tile>(glm::vec2(m_TileSize), TileType::SurfaceTile);
                    break;

                case '3':
                    m_Map[flippedRow][col] = 3;
                    if (!m_Tiles[3])
                        m_Tiles[3] = CreateRef<Tile>(glm::vec2(m_TileSize), TileType::FloatingLeftTile);
                    break;

                case '4':
                    m_Map[flippedRow][col] = 4;
                    if (!m_Tiles[4])
                        m_Tiles[4] = CreateRef<Tile>(glm::vec2(m_TileSize), TileType::FloatingRightTile);
                    break;

                case '5':
                    m_Map[flippedRow][col] = 5;
                    if (!m_Tiles[5])
                        m_Tiles[5] = CreateRef<Tile>(glm::vec2(m_TileSize), TileType::FloatingTile);
                    break;

                case 'P':
                    m_Player.SetPosition(flippedRow, col);
                    m_Map[flippedRow][col] = 0;
                    break;

                default:
                    AB_ASSERT(obj == ' ', "Unknown object type in map!");
                    m_Map[flippedRow][col] = 0;
            }
        }
    }
}
