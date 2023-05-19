///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef GEOMETRY_PASS

struct Light
{
	uint type;
	vec3 color;
	vec3 direction;
	vec3 position;
	float cutoff;
	float outerCutoff;
	float intensity;
	uint isActive;
};

struct Material
{
	float diffuse;
	float specular;
	float shininess;
};

#if defined(VERTEX) ///////////////////////////////////////////////////

// layout(location=x) means the attribute pointer id we set on "glVertexAttribPointer(x, bla, bla, bla, blabla, bla));"
layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoord;

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	float ambient;
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
	vNormal     = normalize(vec3( uWorldMatrix * vec4(aNormal, 0.0) ));
	vViewDir    = normalize(uCameraPosition - vPosition);
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

uniform Material uMaterial;


layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	float ambient;
	uint uLightCount;
	Light uLight[16];
};

layout(location=0) out vec4 fragColor;

vec3 DirectionalLight(in Light light, in vec3 texColor)
{
	vec3 ret = vec3(0);
	vec3 lightDir = normalize(light.direction);

	float specular = 0.5;
	vec3 reflectDir = reflect(-lightDir, vNormal);

	float diffuse = max(dot(vNormal, lightDir), 0.0);
	float spec = pow(max(dot(vViewDir, reflectDir), 0.0), 32);

	ret += ambient * light.color; 
	ret += diffuse * light.color * light.intensity;
	ret += specular * spec * light.color * light.intensity;

	return ret * texColor;
}

vec3 PointLight(in Light light, in vec3 texColor)
{
	float constant = 1;
	float linear = 0.09;
	float quadratic = 0.032;
	float distance  = length(light.position - vPosition);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));  

	vec3 ret = vec3(0);
	vec3 lightDir = normalize(light.position - vPosition);

	float specular = 0.5;
	vec3 reflectDir = reflect(-lightDir, vNormal);

	float diffuse = max(dot(vNormal, lightDir), 0.0);
	float spec = pow(max(dot(vViewDir, reflectDir), 0.0), 32);

	ret += ambient * light.color * attenuation; 
	ret += diffuse * light.color  * attenuation * light.intensity;
	ret += specular * spec * light.color  * attenuation* light.intensity;

	return ret * texColor;
}

vec3 SpotLight(in Light light, in vec3 texColor)
{
	vec3 lightDir = normalize(light.position - vPosition);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutoff - light.outerCutoff;
	float softness = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

	if (theta < light.outerCutoff) return (ambient * light.color) * texColor;

	vec3 ret = vec3(0);
	float specular = 0.5;
	vec3 reflectDir = reflect(-lightDir, vNormal);

	float diffuse = max(dot(vNormal, lightDir), 0.0);
	float spec = pow(max(dot(vViewDir, reflectDir), 0.0), 32);

	ret += ambient * light.color; 
	ret += diffuse * light.color * softness * light.intensity;
	ret += specular * spec * light.color * softness * light.intensity;

	return ret * texColor;
}

void main()
{
	vec4 tex = texture(uTexture, vTexCoord);
	vec3 texColor = vec3(tex);
	vec3 color = vec3(0);
	bool anyLightActive = false;

	for (uint i = 0; i < uLightCount; ++i)
	{
		if (uLight[i].isActive == 0) continue;
		Light light = uLight[i];
		anyLightActive = true;

		switch(light.type)
		{
			case 1: color += DirectionalLight(light, texColor); break;
			case 2: color += PointLight(light, texColor); break;
			case 3: color += SpotLight(light, texColor); break;
		}
	}

	if (!anyLightActive) color += (ambient * vec3(1)) * texColor;

	fragColor = vec4(color, 1.0);
}

#endif ///////////////////////////////////////////////
#endif