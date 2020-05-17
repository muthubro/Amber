#type vertex
#version 430 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Binormal;
layout(location = 4) in vec2 a_TexCoord;

out vec2 v_TexCoord;
    
uniform mat4 u_MVP;

void main() 
{
    v_TexCoord = a_TexCoord;
    gl_Position = u_MVP * vec4(a_Position, 1.0);
}

#type fragment
#version 430 core

in vec2 v_TexCoord;

layout(location = 0) out vec4 color;

uniform sampler2D u_Texture;

void main() 
{
    color = texture(u_Texture, v_TexCoord);
}