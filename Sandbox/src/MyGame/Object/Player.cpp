#include "Player.h"

#include "MyGame/Color.h"
#include "MyGame/Level.h"

Player::Player(const glm::vec2& size, Level* level)
{
	m_Size = { size.x * 2.0f, size.y * 4.0f };
	m_AABB.HalfSize = { m_Size.x * 0.5f - 2.0f, m_Size.y * 0.55f * 0.5f };
	m_AABBOffset = m_AABB.HalfSize - m_Size * 0.5f;
	m_Type = CharacterType::Player;

	m_State = CharacterState::Standing;
	m_KeyFrameID = 0;

	m_Texture = Texture2D::Create("assets/textures/spritesheet_players.png");

	m_TexCoords[0][0] = { 128.0f		  , 1792.0f			};
	m_TexCoords[0][1] = { 128.0f + 128.0f , 1792.0f			};
	m_TexCoords[0][2] = { 128.0f + 128.0f , 1792.0f + 256.0f };
	m_TexCoords[0][3] = { 128.0f		  , 1792.0f + 256.0f };

	m_TexCoords[1][0] = { 128.0f		  , 1024.0f			};
	m_TexCoords[1][1] = { 128.0f + 128.0f , 1024.0f			};
	m_TexCoords[1][2] = { 128.0f + 128.0f , 1024.0f + 256.0f };
	m_TexCoords[1][3] = { 128.0f		  , 1024.0f + 256.0f };

	m_TexCoords[2][0] = { 128.0f		  , 768.0f			};
	m_TexCoords[2][1] = { 128.0f + 128.0f , 768.0f			};
	m_TexCoords[2][2] = { 128.0f + 128.0f , 768.0f + 256.0f };
	m_TexCoords[2][3] = { 128.0f		  , 768.0f + 256.0f };

	m_TexCoords[3][0] = { 256.0f		  , 0.0f		  };
	m_TexCoords[3][1] = { 256.0f + 128.0f , 0.0f		  };
	m_TexCoords[3][2] = { 256.0f + 128.0f , 0.0f + 256.0f };
	m_TexCoords[3][3] = { 256.0f		  , 0.0f + 256.0f };

	m_Level = level;
}

void Player::OnEvent(Event& e)
{
	if (e.GetEventType() == EventType::KeyPressed)
	{
		KeyPressedEvent& keyEvent = static_cast<KeyPressedEvent&>(e);
		if (keyEvent.GetKeyCode() == AB_KEY_SPACE && m_JumpMode < 2)
		{
			m_State = CharacterState::Jumping;
			m_JumpMode++;
			m_Velocity.y = std::min(m_Velocity.y + m_JumpSpeed, m_MaxJumpSpeed);
		}
	}
}

void Player::OnRender()
{
	glm::vec2 size = m_FacingRight ? m_Size : glm::vec2(-m_Size.x, m_Size.y);
	
	if (m_State == CharacterState::Jumping)
	{
		m_KeyFrameID = 3;
	}
	else if (m_WalkingAccumulatedTime >= 2.0f * m_AnimationTimestep)
	{
		m_WalkingAccumulatedTime -= 2.0f * m_AnimationTimestep;
		m_KeyFrameID = 1;
	}
	else if (m_WalkingAccumulatedTime >= m_AnimationTimestep)
	{
		m_KeyFrameID = 2;
	}
	else if (m_State == CharacterState::Walking)
	{
		m_KeyFrameID = 1;
	}
	else
	{
		m_KeyFrameID = 0;
	}

	Renderer2D::DrawQuad(m_Position, size, 0.0f, m_Texture, m_TexCoords[m_KeyFrameID]);
}

void Player::OnUpdate(Timestep ts)
{
	if (Input::IsKeyPressed(AB_KEY_D))
	{
		m_Velocity.x = m_MoveSpeed;
		m_FacingRight = true;
	}
	else if (Input::IsKeyPressed(AB_KEY_A))
	{
		m_Velocity.x = -m_MoveSpeed;
		m_FacingRight = false;
	}
	else
	{
		m_Velocity.x = 0.0f;
		m_WalkingAccumulatedTime = 0.0f;
	}

	glm::vec2 oldPosition = m_Position;

	m_Position.x += m_Velocity.x * ts;
	m_Position.x = glm::clamp(m_Position.x, 0.0f, m_Level->ColumnToX(m_Level->GetWidth()));

	float wallX = 0.0f;
	if (m_Velocity.x >= 0.0f && HasWall(wallX))
	{
		if (oldPosition.x + m_AABB.HalfSize.x + m_AABBOffset.x <= wallX)
		{
			m_Position.x = wallX - m_AABB.HalfSize.x - m_AABBOffset.x;
		}
	}

	if (m_Velocity.x <= 0.0f && HasWall(wallX))
	{
		if (oldPosition.x - m_AABB.HalfSize.x + m_AABBOffset.x >= wallX)
		{
			m_Position.x = wallX + m_AABB.HalfSize.x - m_AABBOffset.x;
		}
	}

	m_Position.y += m_Velocity.y * ts;
	m_Position.y = glm::clamp(m_Position.y, 0.0f, m_Level->RowToY(m_Level->GetHeight()));

	float groundY = 0.0f;
	if (m_Velocity.y <= 0.0f && HasGround(groundY))
	{
		if (std::abs(m_Velocity.x) > 0.0f)
		{
			m_State = CharacterState::Walking;
			m_WalkingAccumulatedTime += ts;
		}
		else
		{
			m_State = CharacterState::Standing;
		}
		m_JumpMode = 0;
		m_Velocity.y = 0.0f;
		m_Position.y = groundY + m_AABB.HalfSize.y - m_AABBOffset.y;
	}
	else
	{
		m_State = CharacterState::Jumping;
		if (!m_JumpMode) m_JumpMode = 1;
		m_Velocity.y = std::max(m_Velocity.y - m_Gravity * ts, -300.0f);
	}

	float ceilingY = 0.0f;
	if (m_Velocity.y > 0.0f && HasCeiling(ceilingY))
	{
		m_Velocity.y = 0.0f;
		m_Position.y = ceilingY - m_AABB.HalfSize.y - m_AABBOffset.y;
	}
}

bool Player::HasGround(float& groundY)
{
	auto center = m_Position + m_AABBOffset;

	auto bottomLeft = center - m_AABB.HalfSize - glm::vec2(0.0f, 1.0f) + glm::vec2(1.0f, 0.0f);
	auto bottomRight = bottomLeft + glm::vec2(m_AABB.HalfSize.x * 2.0f - 2.0f, 0.0f);

	auto tileSize = m_Level->GetTileSize();
	for (auto tile = bottomLeft; ; tile.x += tileSize)
	{
		tile.x = std::min(tile.x, bottomRight.x);

		auto [row, col] = m_Level->PositionToTile(tile);

		groundY = (float)(row + 0.5) * tileSize;

		auto obj = m_Level->GetTile(row, col);

		if (obj && obj->IsGround())
			return true;

		if (tile.x >= bottomRight.x)
			break;
	}

	return false;
}

bool Player::HasCeiling(float& ceilingY)
{
	auto center = m_Position + m_AABBOffset;

	auto topRight = center + m_AABB.HalfSize + glm::vec2(0.0f, 1.0f) - glm::vec2(1.0f, 0.0f);
	auto topLeft = topRight - glm::vec2(m_AABB.HalfSize.x * 2.0f - 2.0f, 0.0f);

	auto tileSize = m_Level->GetTileSize();
	for (auto tile = topLeft; ; tile.x += tileSize)
	{
		tile.x = std::min(tile.x, topRight.x);

		auto [row, col] = m_Level->PositionToTile(tile);

		ceilingY = (float)(row - 0.5) * tileSize;

		auto obj = m_Level->GetTile(row, col);

		if (obj && obj->IsGround())
			return true;

		if (tile.x >= topRight.x)
			break;
	}

	return false;
}

bool Player::HasWall(float& wallX)
{
	auto center = m_Position + m_AABBOffset;

	auto bottom = center + glm::vec2(m_FacingRight ? m_AABB.HalfSize.x + 1.0f : -m_AABB.HalfSize.x - 1.0f, -m_AABB.HalfSize.y);
	auto top = bottom + glm::vec2(0.0f, m_AABB.HalfSize.y * 2.0f);

	auto tileSize = m_Level->GetTileSize();
	for (auto tile = bottom; ; tile.y += tileSize)
	{
		tile.y = std::min(tile.y, top.y);

		auto [row, col] = m_Level->PositionToTile(tile);

		wallX = (float)(m_FacingRight ? col - 0.5f : col + 0.5f) * tileSize;

		auto obj = m_Level->GetTile(row, col);

		if (obj && obj->IsGround())
			return true;

		if (tile.y >= top.y)
			break;
	}

	return false;
}

void Player::SetPosition(uint32_t row, uint32_t col)
{
	SetPosition(m_Level->TileToPosition(row, col) + (m_Size - m_Level->GetTileSize()) * 0.5f);
}

