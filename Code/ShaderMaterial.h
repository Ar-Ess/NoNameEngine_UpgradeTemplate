#pragma once

struct ShaderMaterial
{
	glm::vec3 ambient  = glm::vec3(0.1);
	glm::vec3 diffuse  = glm::vec3(1.0);
	glm::vec3 specular = glm::vec3(0.5);
	float bright = 1;
};