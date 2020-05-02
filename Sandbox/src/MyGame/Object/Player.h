#pragma once

#include <glm/glm.hpp>

#include <Hazel.h>

#include "Character.h"

class Level;

class Player : public Character
{
public:
	Player(const glm::vec2& size, Level* level);

	void OnEvent(Event& e);
	void OnRender();
	void OnUpdate(Timestep ts);

	void SetPosition(const glm::vec2& position) override { m_Position = position; m_AABB.Center = position + m_AABBOffset; }
	void SetPosition(uint32_t row, uint32_t col);

private:
	float m_MoveSpeed = 120.0f;
	glm::vec2 m_Velocity = { 0.0f, 0.0f };
	float m_JumpSpeed = 200.0f;
	float m_MaxJumpSpeed = 300.0f;
	uint8_t m_JumpMode = 0;
	float m_AnimationTimestep = 0.5f;
	inline static float m_Gravity = 400.0f;
	
	bool m_FacingRight = true;

	float m_WalkingAccumulatedTime = 0.0f;
	uint8_t m_KeyFrameID;
	glm::vec2 m_TexCoords[4][4];
	Ref<Texture2D> m_Texture;

	Level* m_Level;

	bool HasGround(float& groundY);
	bool HasCeiling(float& ceilingY);
	bool HasWall(float& wallX);
};
