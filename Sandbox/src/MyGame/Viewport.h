#pragma once

#include <Amber.h>

struct ViewportBounds
{
	float Left, Right;
	float Bottom, Top;

	float GetWidth() { return Right - Left; }
	float GetHeight() { return Top - Bottom; }
};

class Viewport
{
public:
	Viewport(float left, float right, float bottom, float top);

	const ViewportBounds& GetBounds() const { return m_Bounds; }
	const Amber::Scope<Amber::OrthographicCamera>& GetCamera() const { return m_Camera; }

	void SetBounds(const ViewportBounds& bounds);
	void SetPosition(float x, float y, float maxWidth, float maxHeight);

private:
	Amber::Scope<Amber::OrthographicCamera> m_Camera;

	ViewportBounds m_Bounds;
};