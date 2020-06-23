#type vertex
#version 440 core

layout(location = 0) in vec2 a_Position;

out vec3 v_TexCoords;

uniform mat4 u_InverseVP;

void main()
{
	vec4 position = vec4(a_Position.xy, 1.0, 1.0);
	gl_Position = position;

	v_TexCoords = (u_InverseVP * position).xyz;
}

#type fragment
#version 440 core

in vec3 v_TexCoords;

out vec4 o_Color;

uniform samplerCube u_Texture;
uniform float u_TextureLod;
uniform float u_Rotation;

vec3 RotateAboutY(float angle, vec3 vec)
{
	angle = radians(angle);
	mat3 transform = { 
		vec3(cos(angle), 0.0, sin(angle)),
		vec3(	0.0    , 1.0,	0.0     ),
		vec3(-sin(angle), 0.0, cos(angle))
	};

	return transform * vec;
}

void main()
{
	o_Color = textureLod(u_Texture, RotateAboutY(u_Rotation, v_TexCoords), u_TextureLod);
}