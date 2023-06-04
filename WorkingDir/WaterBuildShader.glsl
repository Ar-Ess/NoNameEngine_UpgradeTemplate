///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef WATER_BUILD

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;

layout(binding = 2, std140) uniform GlobalParams
{
	mat4 uViewMatrix;
	mat4 uProjMatrix;
	vec3 uCameraPosition;
};

void main()
{

}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

uniform bool uReflection;
uniform sampler2D uTexture;

layout(location=0) out vec4 final;

layout(binding = 2, std140) uniform GlobalParams
{
	mat4 uViewMatrix;
	mat4 uProjMatrix;
	vec3 uCameraPosition;
};

void BuildReflection()
{
	vec4 clippingPlane = vec4(0,  1, 0, 0);
}

void BuildRefraction()
{
	vec4 clippingPlane = vec4(0, -1, 0, 0);
}

void main()
{
	if (uReflection) BuildReflection();
	else BuildRefraction();

	final = vec4(vec3(0), 1);
}

#endif
#endif