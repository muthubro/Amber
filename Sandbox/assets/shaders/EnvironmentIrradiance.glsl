#type compute
#version 450 core

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(binding = 0) uniform samplerCube u_EnvironmentMap;
layout(binding = 0, rgba16f) restrict writeonly uniform imageCube o_IrradianceMap;

const float PI = 3.1415926536;

vec3 GetCubeMapTexCoords()
{
	vec2 st = gl_GlobalInvocationID.xy / vec2(imageSize(o_IrradianceMap));
	vec2 uv = 2.0 * vec2(st.x, 1.0 - st.y) - vec2(1.0);

	vec3 ret;
	if (gl_GlobalInvocationID.z == 0)		ret = vec3(  1.0, uv.y, -uv.x);
	else if (gl_GlobalInvocationID.z == 1)	ret = vec3( -1.0, uv.y,  uv.x);
	else if (gl_GlobalInvocationID.z == 2)	ret = vec3( uv.x,  1.0, -uv.y);
	else if (gl_GlobalInvocationID.z == 3)	ret = vec3( uv.x, -1.0,  uv.y);
	else if (gl_GlobalInvocationID.z == 4)	ret = vec3( uv.x, uv.y,   1.0);
	else if (gl_GlobalInvocationID.z == 5)	ret = vec3(-uv.x, uv.y,  -1.0);

	return normalize(ret);
}

void main()
{
	vec3 N = GetCubeMapTexCoords();
    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 binormal = cross(N, tangent);

	float delta = 0.25;
	int samples = 0;
	vec3 irradiance = vec3(0.0);
	for (float phi = 0; phi < 2 * PI; phi += delta)
	{
		for (float theta = 0; theta < 0.5 * PI; theta += delta)
		{
			vec3 tangentSample;
			tangentSample.x = sin(theta) * cos(phi);
			tangentSample.y = sin(theta) * sin(phi);
			tangentSample.z = cos(theta);

			vec3 sampleVec = tangentSample.x * tangent + tangentSample.y * binormal + tangentSample.z * N;
			irradiance += texture(u_EnvironmentMap, sampleVec).rgb;
			samples++;			
		}
	}
	irradiance = PI * irradiance / float(samples);

	imageStore(o_IrradianceMap, ivec3(gl_GlobalInvocationID), vec4(irradiance, 1.0));
}