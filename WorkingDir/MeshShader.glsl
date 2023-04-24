///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef TEXTURED_GEOMETRY

#if defined(VERTEX) ///////////////////////////////////////////////////

// layout(location=x) means the attribute pointer id we set on "glVertexAttribPointer(x, bla, bla, bla, blabla, bla));"
layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoord;

layout(binding=1, std140) uniform LocalParams
{
	mat4 uWorldMatrix;
	mat4 uWorldViewProjectionMatrix;
};

out vec2 vTexCoord;
out vec3 vPosition;
out vec3 vNormal;

void main()
{
	vTexCoord = aTexCoord;
	vPosition = vec3( uWorldMatrix * vec4(aPosition, 1.0) );
	vNormal   = vec3( uWorldMatrix * vec4(aNormal, 0.0) );

	gl_Position = uWorldViewProjectionMatrix * vec4(aPosition, 5.0f);

	//gl_Position = vec4(aPosition, 5.0f);
	//gl_Position.z = -gl_Position.z;
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

in vec2 vTexCoord;

// getting the id of the texture using -> glGetUniformLocation(texturedGeometryProgram.handle, "uTexture");
// sending the texture data glUniform1i(app->programUniformTexture, 0);
uniform sampler2D uTexture;

layout(location=0) out vec4 fragColor;

void main()
{
	fragColor = texture(uTexture, vTexCoord);
}

#endif ///////////////////////////////////////////////
#endif