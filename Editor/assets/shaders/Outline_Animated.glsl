#type vertex
#version 440 core

layout(location = 0) in vec3 a_Position;
layout(location = 5) in ivec4 a_BoneIndices;
layout(location = 6) in vec4 a_BoneWeights;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

uniform mat4 u_BoneTransform[100];

void main()
{
	mat4 boneTransform = u_BoneTransform[a_BoneIndices[0]] * a_BoneWeights[0];
	boneTransform += u_BoneTransform[a_BoneIndices[1]] * a_BoneWeights[1];
	boneTransform += u_BoneTransform[a_BoneIndices[2]] * a_BoneWeights[2];
	boneTransform += u_BoneTransform[a_BoneIndices[3]] * a_BoneWeights[3];

	gl_Position = u_ViewProjection * u_Transform * boneTransform * vec4(a_Position, 1.0);
}

#type fragment
#version 440 core

out vec4 o_Color;

uniform vec3 u_Color;

void main()
{
	o_Color = vec4(u_Color, 1.0);
}