#include "Player.h"

#include "MyGame/Level.h"

Player::Player(const glm::vec2& size, Level* level)
{
	m_Size = size;
	m_AABB.HalfSize = glm::vec2(size.x * 0.3f, size.y) * 0.5f;
	m_AABBOffset = size * 0.5f;
	m_Type = GameObjectType::Player;
	m_State = CharacterState::Standing;
	m_Texture = Texture2D::Create("assets/textures/Player.png");

	m_Level = level;
}

void Player::OnEvent(Event& e)
{
	if (e.GetEventType() == EventType::KeyPressed)
	{
		KeyPressedEvent& keyEvent = static_cast<KeyPressedEvent&>(e);
		if (keyEvent.GetKeyCode() == HZ_KEY_SPACE && m_JumpMode < 2)
		{
			m_State = CharacterState::Jumping;
			m_JumpMode++;
			m_Velocity.y += m_JumpSpeed;
		}
	}
}

void Player::OnRender()
{
	glm::vec2 size = m_FacingRight ? m_Size : glm::vec2(-m_Size.x, m_Size.y);
	Renderer2D::DrawQuad(m_Position, size, 0.0f, m_Texture, 1.0f);
}

void Player::OnUpdate(Timestep ts)
{
	if (Input::IsKeyPressed(HZ_KEY_D))
	{
		if (!m_PushingRight) m_Velocity.x = m_MoveSpeed;
		m_FacingRight = true;
	}
	else if (Input::IsKeyPressed(HZ_KEY_A))
	{
		if (!m_PushingLeft) m_Velocity.x = -m_MoveSpeed;
		m_FacingRight = false;
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
			m_PushingRight = true;
		}
	}
	else
	{
		m_PushingRight = false;
	}

	if (m_Velocity.x <= 0.0f && HasWall(wallX))
	{
		if (oldPosition.x - m_AABB.HalfSize.x + m_AABBOffset.x >= wallX)
		{
			m_Position.x = wallX + m_AABB.HalfSize.x - m_AABBOffset.x;
			m_PushingLeft = true;
		}
	}
	else
	{
		m_PushingLeft = false;
	}

	m_Position.y += m_Velocity.y * ts;
	m_Position.y = glm::clamp(m_Position.y, 0.0f, m_Level->RowToY(m_Level->GetHeight()));

	float groundY = 0.0f;
	if (m_Velocity.y <= 0.0f && HasGround(groundY))
	{
		m_State = CharacterState::Standing;
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

	m_Velocity.x = 0.0f;
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

		groundY = (float)(row + 1) * tileSize;

		auto obj = m_Level->GetTile(row, col);

		if (obj && obj->IsGround())
			return true;

		if (tile.x >= bottomRight.x)
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

		wallX = (float)(m_FacingRight ? col : col + 1) * tileSize;

		auto obj = m_Level->GetTile(row, col);

		if (obj && obj->IsGround())
			return true;

		if (tile.y >= top.y)
			break;
	}

	return false;
}
