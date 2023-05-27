///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef LIGHTING_PASS

struct Light
{
	uint type;
	vec3 color;
	vec3 direction;
	vec3 position;
	float cutoff;
	float outerCutoff;
	float intensity;
	bool isActive;
	float bloomThreshold;
};

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location=0) in vec3 aPosition;
layout(location=1) in vec2 aTexCoord;

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	float ambient;
	float near;
	float far;
	float threshold;
	bool blackwhite;
	uint uLightCount;
	Light uLight[16];
};

out vec2 vTexCoord;

void main()
{
	vTexCoord = aTexCoord;
	gl_Position = vec4(aPosition, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

uniform sampler2D gSpecular;
uniform sampler2D gNormals;
uniform sampler2D gPosition;
uniform sampler2D gAlbedo;
uniform sampler2D gDepth;

layout(location=0) out vec4 final;
layout(location=1) out vec4 specular;
layout(location=2) out vec4 normals;
layout(location=3) out vec4 position;
layout(location=4) out vec4 albedo;
layout(location=5) out vec4 light;
layout(location=6) out vec4 bloom;

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	float ambient;
	float near;
	float far;
	float threshold;
	bool blackwhite;
	uint uLightCount;
	Light uLight[16];
};

vec3 vPosition;
vec3 vNormal;
vec3 vViewDir;
in vec2 vTexCoord;

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

float ComputeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // back to NDC 
    float endDepth = (2.0 * near * far) / (far + near - z * (far - near));
	return endDepth / far;
}

float max3(vec3 v) { return max(max(v.x, v.y), v.z); }

vec4 CalculateLightOnly(float thrhld, vec3 clr)
{
	vec3 lightOnly = max(clr/vec3(albedo) - vec3(thrhld), 0);
	if (blackwhite) lightOnly = vec3(max3(lightOnly));
	return vec4(lightOnly, 1);
}

void main()
{
	albedo   = texture(gAlbedo  , vTexCoord);
	specular = texture(gSpecular, vTexCoord);
	normals  = texture(gNormals , vTexCoord);
	position = texture(gPosition, vTexCoord);
	gl_FragDepth = ComputeDepth(texture(gDepth, vTexCoord).x);

	vNormal   = vec3(normals);
	vPosition = vec3(position);
	vViewDir  = normalize(uCameraPosition - vPosition);

	vec3 color = vec3(0);
	bool anyLightActive = false;

	for (uint i = 0; i < uLightCount; ++i)
	{
		if (!uLight[i].isActive) continue;
		Light light = uLight[i];
		vec3 result = vec3(0);
		anyLightActive = true;

		switch(light.type)
		{
			case 1: result += DirectionalLight(light, vec3(albedo)); break;
			case 2: result += PointLight(light, vec3(albedo)); break;
			case 3: result += SpotLight(light, vec3(albedo)); break;
		}

		bloom += CalculateLightOnly(light.bloomThreshold, result);
		color += result;
	}

	light = CalculateLightOnly(threshold, color);

	if (!anyLightActive) color += (ambient * vec3(1)) * vec3(albedo);

	final = vec4(color, 1);
}

#endif ///////////////////////////////////////////////
#endif