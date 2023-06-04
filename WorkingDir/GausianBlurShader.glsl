///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef GAUSIAN_BLUR

#if defined(VERTEX) ///////////////////////////////////////////////////

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
uniform sampler2D uTexture;
uniform bool horizontal;

float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

layout(location=0) out vec4 fragColor;

void main()
{
	vec2 offset = 1.0 / textureSize(uTexture, 0); // gets size of single texel
    vec3 result = texture(uTexture, vTexCoord).rgb * weight[0]; // current fragment's contribution

    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(uTexture, vTexCoord + vec2(offset.x * i, 0.0)).rgb * weight[i];
            result += texture(uTexture, vTexCoord - vec2(offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(uTexture, vTexCoord + vec2(0.0, offset.y * i)).rgb * weight[i];
            result += texture(uTexture, vTexCoord - vec2(0.0, offset.y * i)).rgb * weight[i];
        }
    }

    fragColor = vec4(result, 1.0);
}

#endif
#endif