#include "Viewport.h"

Viewport::Viewport(float left, float right, float bottom, float top)
	: m_Bounds({ left, right, bottom, top })
{
	m_Camera = Hazel::CreateScope<Hazel::OrthographicCamera>(left, right, bottom, top);
}

void Viewport::SetBounds(const ViewportBounds& bounds)
{
	m_Bounds = bounds;
	m_Camera = Hazel::CreateScope<Hazel::OrthographicCamera>(bounds.Left, bounds.Right, bounds.Bottom, bounds.Top);
}

void Viewport::SetPosition(float x, float y, float maxWidth, float maxHeight)
{
	float width = std::min(m_Bounds.GetWidth(), maxWidth);

	float minLeft = 0.0f;
	float maxLeft = x - width * 0.5f;
	float minRight = x + width * 0.5f;
	float maxRight = maxWidth;
	
	if (x - width * 0.5f <= 0.0f) minRight = width;
	if (x + width * 0.5f >= maxWidth) maxLeft = maxWidth - width;
	
	m_Bounds.Left = std::max(minLeft, maxLeft);
	m_Bounds.Right = std::min(minRight, maxRight);

	float height = std::min(m_Bounds.GetHeight(), maxHeight);
	
	float minBottom = 0.0f;
	float maxBottom = y - height * 0.5f;
	float minTop = y + height * 0.5f;
	float maxTop = maxHeight;
	
	if (y - height * 0.5f <= 0.0f) minTop = height;
	if (y + height * 0.5f >= maxHeight) maxBottom = maxHeight - height;
	
	m_Bounds.Bottom = std::max(minBottom, maxBottom);
	m_Bounds.Top = std::min(minTop, maxTop);

	m_Camera = Hazel::CreateScope<Hazel::OrthographicCamera>(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top);
}
