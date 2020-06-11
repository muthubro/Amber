// Simple shader to draw a line

#type vertex
#version 440 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

out vec4 v_Color;

uniform mat4 u_ViewProjection;

void main()
{
	v_Color = a_Color;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0f);
}

#type fragment
#version 440 core

in vec4 v_Color;

layout(location = 0) out vec4 color;

void main()
{
	color = v_Color;
}