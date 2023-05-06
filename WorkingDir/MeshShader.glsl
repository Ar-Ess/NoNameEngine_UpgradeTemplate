///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef TEXTURED_GEOMETRY

struct Light
{
	uint type;
	vec3 color;
	vec3 direction;
	vec3 position;
};

#if defined(VERTEX) ///////////////////////////////////////////////////

// layout(location=x) means the attribute pointer id we set on "glVertexAttribPointer(x, bla, bla, bla, blabla, bla));"
layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoord;

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	uint uLightCount;
	Light uLight[16];
};

layout(binding = 1, std140) uniform LocalParams
{
	mat4 uWorldMatrix;
	mat4 uGlobalMatrix;
};

out vec2 vTexCoord;
out vec3 vPosition;
out vec3 vNormal;
out vec3 vViewDir;


void main()
{
	vTexCoord   = aTexCoord;
	vPosition   = vec3( uWorldMatrix * vec4(aPosition, 1.0) );
	vNormal     = vec3( uWorldMatrix * vec4(aNormal, 0.0) );
	vViewDir    = uCameraPosition - vPosition;
	gl_Position = uGlobalMatrix * vec4(aPosition, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

in vec2 vTexCoord;
in vec3 vPosition;
in vec3 vNormal;
in vec3 vViewDir;

// getting the id of the texture using -> glGetUniformLocation(texturedGeometryProgram.handle, "uTexture");
// sending the texture data glUniform1i(app->programUniformTexture, 0);
uniform sampler2D uTexture;

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	uint uLightCount;
	Light uLight[16];
};

layout(location=0) out vec4 fragColor;

void main()
{
	fragColor = texture(uTexture, vTexCoord);
}

#endif ///////////////////////////////////////////////
#endif