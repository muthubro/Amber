#type vertex
#version 440 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_Transform;
uniform mat4 u_ViewProjection;

void main()
{
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 440 core

out vec4 o_Color;

uniform vec3 u_Albedo;

void main()
{
	o_Color = vec4(u_Albedo, 1.0);
}