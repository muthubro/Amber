#pragma once

#include <glm/glm.hpp>

#include <Amber.h>

#include "GameObject.h"

using namespace Amber;

enum class TileType : uint32_t
{
    Empty               = 0,
    GroundTile          = (uint32_t)GameObjectType::Block | BIT(8),
    SurfaceTile         = (uint32_t)GameObjectType::Block | BIT(9),
    FloatingTile        = (uint32_t)GameObjectType::Block | BIT(10),
    FloatingLeftTile    = (uint32_t)GameObjectType::Block | BIT(11),
    FloatingRightTile   = (uint32_t)GameObjectType::Block | BIT(12),
    Count               = 6
};

class Tile : public GameObject
{
public:
    Tile(const glm::vec2& size, const TileType& type);
    virtual ~Tile() = default;

    void OnRender(const glm::vec2& position);

    virtual const TileType& GetType() const { return m_Type; }
    virtual void SetType(const TileType& type) { m_Type = type; }

    virtual bool IsGround() const { return (uint32_t)m_Type & (uint32_t)GameObjectType::Block; }

protected:
    TileType m_Type;
    Ref<Texture2D> m_Texture;
    glm::vec2 m_TexCoords[4];
};
