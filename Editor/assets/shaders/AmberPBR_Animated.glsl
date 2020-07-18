#type vertex
#version 440 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Binormal;
layout(location = 5) in ivec4 a_BoneIndices;
layout(location = 6) in vec4 a_BoneWeights;

out VertexOutput
{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoord;
	vec3 ViewPos;
	vec3 LightDir;
} vs_Output;

uniform vec3 u_ViewPosition;
uniform vec3 u_LightDirection;

uniform mat3 u_NormalTransform;
uniform mat4 u_Transform;
uniform mat4 u_ViewProjection;

const uint MAX_BONES = 100;
uniform mat4 u_BoneTransform[100];

uniform bool u_NormalTexToggle;

void main()
{
	mat4 boneTransform = u_BoneTransform[a_BoneIndices[0]] * a_BoneWeights[0];
	boneTransform += u_BoneTransform[a_BoneIndices[1]] * a_BoneWeights[1];
	boneTransform += u_BoneTransform[a_BoneIndices[2]] * a_BoneWeights[2];
	boneTransform += u_BoneTransform[a_BoneIndices[3]] * a_BoneWeights[3];

	vec4 worldPos = u_Transform  * boneTransform * vec4(a_Position, 1.0);
	vec3 N = a_Normal;

	vs_Output.Normal = u_NormalTransform * mat3(boneTransform) * N;
	vs_Output.TexCoord = a_TexCoords;
	if (u_NormalTexToggle)
	{
		vec3 T = a_Tangent;
		T = normalize(T - dot(T, N) * N);

		vec3 B = a_Binormal;
		B = normalize(B - dot(B, T) * T - dot(B, N) * N);

		mat3 TBN = inverse(u_NormalTransform * mat3(boneTransform) * mat3(T, B, N));
		vs_Output.FragPos = TBN * vec3(worldPos);
		vs_Output.ViewPos = TBN * u_ViewPosition;
		vs_Output.LightDir = TBN * u_LightDirection;
	}
	else
	{
		vs_Output.FragPos = vec3(worldPos);
		vs_Output.ViewPos = u_ViewPosition;
		vs_Output.LightDir = u_LightDirection;
	}

	gl_Position = u_ViewProjection * worldPos;
}

#type fragment
#version 440 core

in VertexOutput
{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoord;
	vec3 ViewPos;
	vec3 LightDir;
} fs_Input;

out vec4 o_Color;

struct Light
{
	vec3 Radiance;
	float Multiplier;
};
uniform Light u_Light;

uniform sampler2D u_AlbedoTexture;
uniform sampler2D u_NormalTexture;
uniform sampler2D u_MetalnessTexture;
uniform sampler2D u_RoughnessTexture;

uniform vec3 u_Albedo;
uniform float u_Metalness;
uniform float u_Roughness;

uniform bool u_AlbedoTexToggle;
uniform bool u_NormalTexToggle;
uniform bool u_MetalnessTexToggle;
uniform bool u_RoughnessTexToggle;

uniform samplerCube u_IrradianceTexture;
uniform samplerCube u_RadianceTexture;
uniform sampler2D u_BRDFLUT;

uniform float u_EnvironmentRotation;

struct PBRParameters
{
	vec3 Albedo;
	vec3 Normal;
	float Metalness;
	float Roughness;

	vec3 View;
	float NdotV;
};
PBRParameters m_Params;

const float PI = 3.1415926536;
const float Epsilon = 0.0000001;

float DistributionGGX(float NdotH, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
    float NdotH2 = clamp(NdotH * NdotH, 0.0, 1.0);

	float denom = NdotH2 * (a2 - 1.0) + 1.0;
	denom = PI * denom * denom;

	return a2 / max(denom, Epsilon);
}

float GeometrySchlick(float NdotV, float roughness)
{
	float r = roughness + 1.0;
	float k = r * r / 8.0;

	return NdotV / ((1.0 - k) * NdotV + k);
}

float GeometrySmith(vec3 N, vec3 L, vec3 V, float roughness)
{
	float NdotL = max(dot(N, L), 0.0);
	float NdotV = max(dot(N, V), 0.0);
	float ggx1 = GeometrySchlick(NdotL, roughness);
	float ggx2 = GeometrySchlick(NdotV, roughness);

	return ggx1 * ggx2;
}

vec3 FresnelSchlick(float HdotV, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - HdotV, 5.0);
}

vec3 FresnelSchlickRoughness(float HdotV, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - HdotV, 5.0);
}

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

vec3 Lighting(vec3 F0)
{
	vec3 L = normalize(fs_Input.LightDir);
	vec3 H = normalize(L + m_Params.View);
	float NdotL = max(dot(m_Params.Normal, L), 0.0);

	float D = DistributionGGX(max(dot(m_Params.Normal, H), 0.0), m_Params.Roughness);
	float G = GeometrySmith(m_Params.Normal, L, m_Params.View, m_Params.Roughness);
	vec3 F = FresnelSchlick(clamp(dot(m_Params.View, H), 0.0, 1.0), F0);

	vec3 k_D = (1.0 - F) * (1.0 - m_Params.Metalness);
	vec3 diffuse = k_D * m_Params.Albedo / PI;
	vec3 specular = D * F * G / max(4.0 * NdotL * m_Params.NdotV, Epsilon);
	vec3 brdf = diffuse + specular; 

	return brdf * u_Light.Radiance * u_Light.Multiplier * NdotL;
}

vec3 IBL(vec3 F0, vec3 R)
{
	vec3 k_S = FresnelSchlickRoughness(m_Params.NdotV, F0, m_Params.Roughness);
	vec3 k_D = (1.0 - k_S) * (1.0 - m_Params.Metalness);
	vec3 irradiance = texture(u_IrradianceTexture, m_Params.Normal).rgb;
	vec3 diffuse = k_D * irradiance * m_Params.Albedo;

	const float MAX_RADIANCE_LOD = textureQueryLevels(u_RadianceTexture) - 1.0;
	vec3 radiance = textureLod(u_RadianceTexture, RotateAboutY(u_EnvironmentRotation, R), m_Params.Roughness * MAX_RADIANCE_LOD).rgb;
	vec2 brdf  = texture(u_BRDFLUT, vec2(m_Params.NdotV, 1.0 - m_Params.Roughness)).rg;
	vec3 specular = radiance * (k_S * brdf.x + brdf.y);

	return diffuse + specular;
}

void main()
{
	m_Params.Albedo    = u_AlbedoTexToggle ? texture(u_AlbedoTexture, fs_Input.TexCoord).rgb : u_Albedo;
	m_Params.Metalness = u_MetalnessTexToggle ? texture(u_MetalnessTexture, fs_Input.TexCoord).r : u_Metalness;
	m_Params.Roughness = u_RoughnessTexToggle ? texture(u_RoughnessTexture, fs_Input.TexCoord).r : u_Roughness;
	m_Params.Roughness = max(m_Params.Roughness, 0.05);

	m_Params.Normal = u_NormalTexToggle ? texture(u_NormalTexture, fs_Input.TexCoord).rgb * 2.0 - 1.0 : normalize(fs_Input.Normal);
	m_Params.View = normalize(fs_Input.ViewPos - fs_Input.FragPos);
	m_Params.NdotV = clamp(dot(m_Params.Normal, m_Params.View), 0.0, 1.0);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, m_Params.Albedo, m_Params.Metalness);
	
	vec3 R = reflect(-m_Params.View, m_Params.Normal);

	vec3 color = vec3(0.0);
	color += Lighting(F0);
	color += IBL(F0, R);

	o_Color = vec4(color, 1.0);
}