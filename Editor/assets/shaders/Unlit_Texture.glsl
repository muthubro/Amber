#type vertex
#version 440 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

uniform mat4 u_Transform;
uniform mat4 u_ViewProjection;

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0f);
}

#type fragment
#version 440 core

in vec2 v_TexCoord;

out vec4 o_Color;

uniform sampler2D u_AlbedoTexture;

void main()
{
	o_Color = texture(u_AlbedoTexture, v_TexCoord);
	o_Color = vec4(texture(u_AlbedoTexture, v_TexCoord).rgb, 1.0);
}