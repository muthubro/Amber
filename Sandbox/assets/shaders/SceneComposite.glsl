#type vertex
#version 440 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoords;

out vec2 v_TexCoords;

void main()
{
	v_TexCoords = a_TexCoords;
	gl_Position = vec4(a_Position.xy, 0.0, 1.0);
}

#type fragment
#version 440 core

in vec2 v_TexCoords;

out vec4 o_Color;

uniform float u_Exposure;
uniform sampler2DMS u_Texture;
uniform int u_TextureSamples;

vec4 MultisampleTexture(sampler2DMS tex, ivec2 texCoords, int samples)
{
	vec4 color = vec4(0.0);
	for (int i = 0; i < samples; i++)
		color += texelFetch(tex, texCoords, i);

	return color / float(samples);
}

void main()
{
	float gamma = 2.2;

	ivec2 texSize = textureSize(u_Texture);
	ivec2 texCoords = ivec2(v_TexCoords * texSize);
	vec3 color = MultisampleTexture(u_Texture, texCoords, u_TextureSamples).rgb;
	color = vec3(1.0) - exp(-color * u_Exposure);

	o_Color = vec4(pow(color, vec3(1.0 / gamma)), 1.0);
}