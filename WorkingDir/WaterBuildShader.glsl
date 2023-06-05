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

void main()
{
	vTexCoord   = aTexCoord;
	gl_Position = uGlobalMatrix * vec4(aPosition, 1.0);

}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

uniform bool uReflection;
uniform sampler2D uTexture;

in vec2 vTexCoord;

layout(location=0) out vec4 final;

layout(binding = 2, std140) uniform GlobalParams
{
	mat4 uWorldMatrix;
	mat4 uGlobalMatrix;
};

void main()
{
	vec3 color = texture(uTexture, vTexCoord).rgb;
	final = vec4(color + vec3(0.05, 0.05, 0.1f), 1);
}

#endif
#endif