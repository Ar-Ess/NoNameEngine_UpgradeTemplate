#pragma once
#include <string.h>
#include "Typedef.h"
#include "Flag.h"

class Material
{
public:

	Flag properties; // Diffuse(0) | Specular(1) | Normals(3) | Bump(4) | Emissive(3)
	std::string name;
	vec3 diffuse = vec3(1.f);
	vec3 emissive = vec3(1.f);
	vec3 specular = vec3(0.5f);
	float shininess = 32;
	
	unsigned int diffuseTex;
	unsigned int emissiveTex;
	unsigned int specularTex;
	unsigned int normalsTex;
	unsigned int bumpTex;

};