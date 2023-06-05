///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef WATER_PASS

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoord;

layout(binding = 2, std140) uniform GlobalParams
{
	mat4 uWorldMatrix;
	mat4 uGlobalMatrix;
	mat4 iGlobalMatrix;
	mat4 uViewMatrix;
	mat4 iViewMatrix;
};

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoord;

void main()
{
	vTexCoord   = aTexCoord;
	vPosition   = vec3( uWorldMatrix * vec4(aPosition, 1.0) );
	vNormal     = normalize(vec3( uWorldMatrix * vec4(aNormal, 0.0) ));
	gl_Position = uGlobalMatrix * vec4(vPosition, 1.0); // usually it is an aPosition
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

layout(binding = 2, std140) uniform GlobalParams
{
	mat4 uWorldMatrix;
	mat4 uGlobalMatrix;
	mat4 iGlobalMatrix;
	mat4 uViewMatrix;
	mat4 iViewMatrix;
};

uniform sampler2D uReflectionMap;
uniform sampler2D uRefractionMap;
uniform sampler2D uReflectionDepth;
uniform sampler2D uRefractionDepth;
uniform sampler2D uNormalMap;
uniform sampler2D uDudvMap;

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;

layout(location=0) out vec4 specular;
layout(location=1) out vec4 normals;
layout(location=2) out vec4 position;
layout(location=3) out vec4 albedo;

vec3 FresnelSchlick(float cosTheta, vec3 f0)
{
	return f0 + (1 - f0) * pow(1 - cosTheta, 5.0);
}

vec3 ReconstructPixelPosition(float depth)
{
	vec3 posNDC = vec3(vTexCoord * 2 - vec2(1), depth * 2 - 1);
	vec4 posEyespace = iGlobalMatrix * vec4(posNDC, 1);
	posEyespace.xyz /= posEyespace.w; // TODO: can be simplified by return directly posEyespace.xyz / posEyespace.w
	return posEyespace.xyz;
}

void main()
{
	normals  = vec4(vNormal,   1);
    position = vec4(vPosition, 1);
	specular = vec4(vec3(0.5), 1);
	//gl_FragDepth = gl_FragCoord.z; Not this one, or maybe yes? To determine

	vec3 v  = normalize(-vPosition);
	vec3 pW = vec3(iViewMatrix * vec4(vPosition, 1));

	const vec2 waveLength   = vec2(2);
	const vec2 waveStrength = vec2(0.05);
	const float turbidityDistance = 10;

	vec2 distortion = (2 * texture(uDudvMap, pW.xz / waveLength).rg - vec2(1)) * waveStrength + waveStrength/7;

	// Distorted reflection and refraction
	vec2 reflectionTexCoord = vec2(vTexCoord.s, 1 - vTexCoord.t) + distortion;
	vec2 refractionTexCoord = vTexCoord + distortion;
	vec3 reflectionColor = texture(uReflectionMap, reflectionTexCoord).rgb;
	vec3 refractionColor = texture(uRefractionMap, refractionTexCoord).rgb;

	// Water tint
	float distortedGroundDepth = texture(uRefractionDepth, refractionTexCoord).x;
	vec3  distortedGroundPosViewspace = ReconstructPixelPosition(distortedGroundDepth);
	float distortedWaterDepth = vPosition.z - distortedGroundPosViewspace.z;
	float tintFactor = clamp(distortedWaterDepth / turbidityDistance, 0, 1);
	vec3 waterColor = vec3(0.25, 0.4, 0.6);
	refractionColor = mix(refractionColor, waterColor, tintFactor);

	// Fresnel
	vec3 f0 = vec3(0.1);
	vec3 f  = FresnelSchlick(max(0, dot(v, vNormal)), f0);

	albedo = vec4(mix(refractionColor, reflectionColor, f), 1);

}

#endif
#endif