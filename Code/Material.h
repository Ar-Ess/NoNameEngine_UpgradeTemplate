#pragma once
#include <string.h>
#include "Typedef.h"

class Material
{
public:

	std::string name;
	vec3 albedo = vec3(1.f, 1.f, 1.f);
	vec3 emissive = vec3(1.f, 1.f, 1.f);
	float smoothness;
	unsigned int albedoTex;
	unsigned int emissiveTex;
	unsigned int specularTex;
	unsigned int normalsTex;
	unsigned int bumpTex;

};