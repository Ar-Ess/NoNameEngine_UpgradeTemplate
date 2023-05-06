#pragma once

enum class LightType
{
	LT_NONE,
	LT_DIRECTIONAL,
	LT_POINT
};

struct Light
{

	Light(LightType type, glm::vec3 color, glm::vec3 position, glm::vec3 direction) : 
		type(type), color(color), position(position), direction(glm::normalize(direction))
	{

	}

	LightType type = LightType::LT_NONE;
	glm::vec3 color = glm::vec3(1, 1, 1);
	glm::vec3 direction;
	glm::vec3 position;
};