///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef WATER_BUILD

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoord;

layout(binding = 2, std140) uniform GlobalParams
{
	mat4 uWorldMatrix;
	mat4 uGlobalMatrix;
	vec3 uCameraPosition;
};

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoord;

void main()
{
	vTexCoord   = aTexCoord;
	vPosition   = vec3( uWorldMatrix * vec4(aPosition, 1.0) );
	vNormal     = normalize(vec3( uWorldMatrix * vec4(aNormal, 0.0) ));
	gl_Position = uGlobalMatrix * vec4(aPosition, 1.0);

}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

uniform bool uReflection;
uniform sampler2D uTexture;
uniform float uFactor;

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;

layout(location=0) out vec4 final;

layout(binding = 2, std140) uniform GlobalParams
{
	mat4 uWorldMatrix;
	mat4 uGlobalMatrix;
	vec3 uCameraPosition;
};

void BuildReflection(out vec3 color)
{
	gl_ClipDistance[0] = dot(vec4(uWorldMatrix.xyz, 1.0), vec4(0,  1, 0, 0));
	
	vec3 incident = normalize(vPosition - uCameraPosition);
	
	vec3 reflection = reflect(incident, vNormal);

	color = vec3(texture(uTexture, reflection.xy));
}

void BuildRefraction(out vec3 color)
{
	gl_ClipDistance[0] = dot(vec4(uWorldMatrix.xyz, 1.0), vec4(0, -1, 0, 0));

	vec3 incident = normalize(vPosition - uCameraPosition);

	float cosTheta1 = dot(incident, vNormal);
	float sinTheta1 = sqrt(1 - cosTheta1 * cosTheta1);
	float sinTheta2 = sinTheta1 / uFactor;
	
	vec3 refraction = refract(incident, vNormal, sinTheta2);

	color = vec3(texture(uTexture, refraction.xy));
}

void main()
{
	vec3 color = vec3(0);

	if (uReflection) 
		BuildReflection(color);

	else 
		BuildRefraction(color);

	vec3 sum = vec3(0);
	if (color.x + color.y + color.z == 0) sum = vec3(0, 0, 0.5);

	final = vec4(color + sum, 1);
}

#endif
#endif