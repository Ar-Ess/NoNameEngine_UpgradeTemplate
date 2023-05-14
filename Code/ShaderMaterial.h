#pragma once

struct ShaderMaterial
{
	glm::vec3 ambient  = glm::vec3(1, 0.5, 0.31);
	glm::vec3 diffuse  = glm::vec3(1, 0.5, 0.31);
	glm::vec3 specular = glm::vec3(0.5);
	float bright = 0.32;
};