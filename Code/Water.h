#pragma once
#include "Object.h"
#include "Mesh.h"
#include "glm/glm.hpp"
//jaume

enum class WaterType {
	WT_STILL
};

struct Water : public Object {
	Water(WaterType type, glm::vec3 baseColor, glm::vec3 position,glm::vec3 scale, float refractionFactor,float reflectionFactor,float densityFactor) : type(type), baseColor(baseColor),refractionFactor(refractionFactor),
		reflectionFactor(reflectionFactor),densityFactor(densityFactor), Object(ObjectType::O_WATER) {
		this->position = position;
		this->scale = scale;
	}

	bool DrawGui() override
	{
		bool change = false;

		switch (type)
		{
		case WaterType::WT_STILL:
			if (ImGui::CollapsingHeader("Transform"))
				change = Draw3Float("Position:", &position, 0.1, 0, 0, "X: %.2f", "Y: %.2f", "Z: %.2f");
			break;
		}
		if (ImGui::CollapsingHeader("RefractionFactor"))
		{
			DrawFloat("factor:", &refractionFactor, 0.01, 0, 1, "F: %.2f");
		}
		if (ImGui::CollapsingHeader("ReflectionFactor"))
		{
			DrawFloat("factor:", &reflectionFactor, 0.01, 0, 1, "F: %.2f");
		}
		if (ImGui::CollapsingHeader("DensityFactor"))
		{
			DrawFloat("factor:", &densityFactor, 0.01, 0, 1, "F: %.2f");
		}
		return change;
	}

public:
	glm::vec3 position;
	glm::vec3 scale;

private:
	WaterType type;
	glm::vec3 baseColor;

	float refractionFactor;
	float reflectionFactor;
	float densityFactor;

public:
	// programs[this->forwardProgram]->handle
	GLuint forwardProgram = 0;

	// programs[this->deferredProgram]->handle
	GLuint deferredProgram = 0;

	GLuint texUniformForward = 0;
	GLuint texUniformDeferred = 0;

	std::vector<Mesh*> meshes;
	std::vector<unsigned int> materials;
};