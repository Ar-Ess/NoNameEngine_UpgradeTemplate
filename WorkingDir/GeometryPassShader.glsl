///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef GEOMETRY_PASS

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoord;

layout(binding = 1, std140) uniform LocalParams
{
	mat4 uWorldMatrix;
	mat4 uGlobalMatrix;
};

out vec2 vTexCoord;
out vec3 vPosition;
out vec3 vNormal;

void main()
{
	vTexCoord   = aTexCoord;
	vPosition   = vec3( uWorldMatrix * vec4(aPosition, 1.0) );
	vNormal     = normalize(vec3( uWorldMatrix * vec4(aNormal, 0.0) ));
	gl_Position = uGlobalMatrix * vec4(aPosition, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

layout(location=0) out vec4 specular;
layout(location=1) out vec4 normals;
layout(location=2) out vec4 position;
layout(location=3) out vec4 albedo;

in vec2 vTexCoord;
in vec3 vPosition;
in vec3 vNormal;

uniform sampler2D uTexture;

void main()
{
	albedo   = vec4(vec3(texture(uTexture, vTexCoord)), 1);
    normals  = vec4(vNormal,   1);
    position = vec4(vPosition, 1);
	specular = vec4(vec3(0.5), 1);
	gl_FragDepth = gl_FragCoord.z;
}

#endif ///////////////////////////////////////////////
#endif