#pragma once

#include <glm/glm.hpp>

#include <Hazel.h>

#include "Character.h"
//#include "MyGame/Level.h"

class Level;

class Player : public Character
{
public:
	Player(const glm::vec2& size, Level* level);

	void OnEvent(Event& e);
	void OnRender();
	void OnUpdate(Timestep ts);

private:
	float m_MoveSpeed = 150.0f;
	glm::vec2 m_Velocity = { 0.0f, 0.0f };
	float m_JumpSpeed = 250.0f;
	uint8_t m_JumpMode = 0;
	inline static float m_Gravity = 400.0f;
	
	bool m_FacingRight = true;
	bool m_PushingRight = false;
	bool m_PushingLeft = false;

	Level* m_Level;

	bool HasGround(float& groundY);
	bool HasWall(float& wallX);
};
