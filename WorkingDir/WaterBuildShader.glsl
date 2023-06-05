///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef WATER_BUILD

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location=0) in vec3 aPosition;
layout(location=2) in vec2 aTexCoord;

layout(binding = 2, std140) uniform GlobalParams
{
	mat4 uWorldMatrix;
	mat4 uGlobalMatrix;
};

out vec2 vTexCoord;

uniform bool uReflection;
uniform float uHeight;

void main()
{
	vTexCoord   = aTexCoord;
	vec3 positionWorldSpace = vec3( uWorldMatrix * vec4(aPosition, 1.0) );

	if (uReflection) gl_ClipDistance[0] = dot(vec4(positionWorldSpace, 1.0), vec4(0,  1, 0, -uHeight));
	else gl_ClipDistance[0] = dot(vec4(positionWorldSpace, 1.0), vec4(0,  -1, 0, uHeight));

	gl_Position = uGlobalMatrix * vec4(aPosition, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

uniform sampler2D uTexture;

in vec2 vTexCoord;

layout(location=0) out vec4 albedo;

layout(binding = 2, std140) uniform GlobalParams
{
	mat4 uWorldMatrix;
	mat4 uGlobalMatrix;
};

void main()
{
	vec3 color = texture(uTexture, vTexCoord).rgb;
	albedo = vec4(color + vec3(0.05, 0.05, 0.1f), 1);
}

#endif
#endif