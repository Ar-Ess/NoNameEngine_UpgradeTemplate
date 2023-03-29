///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef TEXTURED_GEOMETRY

#if defined(VERTEX) ///////////////////////////////////////////////////

// layout(location=x) means the attribute pointer id we set on ""glVertexAttribPointer(x, bla, bla, bla, blabla, bla));"
layout(location=0) in vec3 aPosition;
layout(location=2) in vec2 aTexCoord;

out vec2 vTexCoord;

void main()
{
	vTexCoord = aTexCoord;

	float clippingScale = 5.0;
	gl_Position = vec4(aPosition, clippingScale);

	fl_Position.z = -gl_Position.z;
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

#endif
#endif