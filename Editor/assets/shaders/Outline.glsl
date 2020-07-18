#type vertex
#version 440 core

layout(location = 0) in vec3 a_Position;

out vec3 v_Position;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main()
{
	vec4 position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
	v_Position = vec3(position);
	gl_Position = position;
}

#type fragment
#version 440 core

in vec3 v_Position;

out vec4 o_Color;

uniform vec3 u_Color;
uniform vec2 u_Center;
uniform vec2 u_Size;;
uniform float u_Radius;
uniform bool u_Border;

void main()
{
	if (u_Border)
	{
		if (length(max(abs(vec2(v_Position) - u_Center) - u_Size * 0.5f, 0.0)) > u_Radius)
			discard;
	}

	o_Color = vec4(u_Color, 1.0);
}