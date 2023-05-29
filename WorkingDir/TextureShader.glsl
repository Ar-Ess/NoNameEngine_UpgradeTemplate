///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef TEXTURED_GEOMETRY

#if defined(VERTEX) ///////////////////////////////////////////////////

// layout(location=x) means the attribute pointer id we set on ""glVertexAttribPointer(x, bla, bla, bla, blabla, bla));"
layout(location=0) in vec3 aPosition;
layout(location=1) in vec2 aTexCoord;

out vec2 vTexCoord;

void main()
{
	vTexCoord = aTexCoord;
	gl_Position = vec4(aPosition, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

in vec2 vTexCoord;

// getting the id of the texture using -> glGetUniformLocation(texturedGeometryProgram.handle, "uTexture");
// sending the texture data glUniform1i(app->programUniformTexture, 0);
uniform sampler2D uTexture;
uniform sampler2D uBloom;
float exposure = 0.5;

layout(location=0) out vec4 fragColor;

void main()
{
	const float gamma = 2.2;
	vec3 tex = texture(uTexture, vTexCoord).rgb;
	vec3 bloom = texture(uBloom, vTexCoord).rgb;

	if (bloom.x + bloom.y + bloom.z == 0)
	{
		fragColor = vec4(tex, 1);
		return;
	}

	// Additive blending
	tex += bloom;

    // Tone mapping
    vec3 result = vec3(1.0) - exp(-tex * exposure);

    // Gamma correction     
    result = pow(result, vec3(1.0 / gamma));
    fragColor = vec4(result, 1.0);
}

#endif
#endif