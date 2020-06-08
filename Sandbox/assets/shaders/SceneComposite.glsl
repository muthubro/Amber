#type vertex
#version 450 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoords;

out vec2 v_TexCoords;

void main()
{
	v_TexCoords = a_TexCoords;
	gl_Position = vec4(a_Position.xy, 0.0, 1.0);
}

#type fragment
#version 450 core

in vec2 v_TexCoords;

out vec4 o_Color;

uniform sampler2D u_Texture;
uniform float u_Exposure;

void main()
{
	float gamma = 2.2;

	vec3 color = texture(u_Texture, v_TexCoords).rgb;
	color = vec3(1.0) - exp(-color * u_Exposure);

	o_Color = vec4(pow(color, vec3(1.0 / gamma)), 1.0);
}