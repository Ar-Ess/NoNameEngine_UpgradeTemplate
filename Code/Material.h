#pragma once
#include <string.h>

class Material
{
public:

	std::string name;
	vec3 albedo;
	vec3 emissive;
	float smoothness;
	unsigned int albedoTex;
	unsigned int emissiveTex;
	unsigned int specularTex;
	unsigned int normalsTex;
	unsigned int bumpTex;

};