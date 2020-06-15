#type vertex
#version 440 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;

out vec2 v_TexCoords;

uniform mat4 u_Transform;
uniform mat4 u_ViewProjection;

void main()
{
	v_TexCoords = a_TexCoords;
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0f);
}

#type fragment
#version 440 core

in vec2 v_TexCoords;

out vec4 o_Color;

uniform float u_Resolution;
uniform float u_Scale;

float Grid(vec2 st, float res)
{
	vec2 grid = fract(st);
	return step(res, grid.x) * step(res, grid.y);
}

void main()
{
	float empty = Grid(v_TexCoords * u_Scale, u_Resolution);
	o_Color = vec4(vec3(0.5), 1.0) * (1.0 - empty);
}