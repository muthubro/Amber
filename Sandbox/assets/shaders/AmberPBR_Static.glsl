#type vertex
#version 440 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec2 a_TexCoords;

out VertexOutput
{
	vec3 v_FragPos;
	vec3 v_Normal;
	vec2 v_TexCoord;
	vec3 v_ViewPos;
	vec3 v_LightDir;
} vsOutput;

uniform vec3 u_ViewPosition;
uniform vec3 u_LightDirection;

uniform mat3 u_NormalTransform;
uniform mat4 u_Transform;
uniform mat4 u_ViewProjection;

uniform bool u_NormalTexToggle;

void main()
{
	vec4 worldPos = u_Transform * vec4(a_Position, 1.0);
	vec3 N = a_Normal;

	vsOutput.v_Normal = u_NormalTransform * N;
	vsOutput.v_TexCoord = a_TexCoords;
	if (u_NormalTexToggle)
	{
		vec3 T = u_NormalTransform * a_Tangent;
		T = normalize(T - dot(T, N) * N);

		vec3 B = cross(N, T);

		mat3 TBN = u_NormalTransform * mat3(T, B, N);
		vsOutput.v_FragPos = TBN * vec3(worldPos);
		vsOutput.v_ViewPos = TBN * u_ViewPosition;
		vsOutput.v_LightDir = TBN * u_LightDirection;
	}
	else
	{
		vsOutput.v_FragPos = vec3(worldPos);
		vsOutput.v_ViewPos = u_ViewPosition;
		vsOutput.v_LightDir = u_LightDirection;
	}

	gl_Position = u_ViewProjection * worldPos;
}

#type fragment
#version 440 core

in VertexOutput
{
	vec3 v_FragPos;
	vec3 v_Normal;
	vec2 v_TexCoord;
	vec3 v_ViewPos;
	vec3 v_LightDir;
} fsInput;

out vec4 o_Color;

struct Light
{
	vec3 Radiance;
	vec3 Multiplier;
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

vec3 Lighting(vec3 F0)
{
	vec3 L = normalize(fsInput.v_LightDir);
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
	vec3 radiance = textureLod(u_RadianceTexture, R, m_Params.Roughness * MAX_RADIANCE_LOD).rgb;
	vec2 brdf  = texture(u_BRDFLUT, vec2(m_Params.NdotV, 1.0 - m_Params.Roughness)).rg;
	vec3 specular = radiance * (k_S * brdf.x + brdf.y);

	return diffuse + specular;
}

void main()
{
	m_Params.Albedo    = u_AlbedoTexToggle ? texture(u_AlbedoTexture, fsInput.v_TexCoord).rgb : u_Albedo;
	m_Params.Metalness = u_MetalnessTexToggle ? texture(u_MetalnessTexture, fsInput.v_TexCoord).r : u_Metalness;
	m_Params.Roughness = u_RoughnessTexToggle ? texture(u_RoughnessTexture, fsInput.v_TexCoord).r : u_Roughness;
	m_Params.Roughness = max(m_Params.Roughness, 0.05);

	m_Params.Normal = u_NormalTexToggle ? texture(u_NormalTexture, fsInput.v_TexCoord).rgb * 2.0 - 1.0 : normalize(fsInput.v_Normal);
	m_Params.View = normalize(fsInput.v_ViewPos - fsInput.v_FragPos);
	m_Params.NdotV = clamp(dot(m_Params.Normal, m_Params.View), 0.0, 1.0);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, m_Params.Albedo, m_Params.Metalness);
	
	vec3 R = reflect(-m_Params.View, m_Params.Normal);

	vec3 color = vec3(0.0);
	color += Lighting(F0);
	color += IBL(F0, R);

	o_Color = vec4(color, 1.0);
}