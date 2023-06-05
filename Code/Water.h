#pragma once
#include "Object.h"
#include "Mesh.h"
#include "WaterBuffer.h"
//jaume
#include "Model.h"
//class Model;

enum class WaterType 
{
	WT_STILL
};

class Water : public Object 
{
public:
	
	Water() : Object(ObjectType::O_WATER) 
	{
	}

	bool DrawGui() override
	{
		bool change = false;

		if (ImGui::CollapsingHeader("Transform"))
		{
			if (Draw3Float("Position:", &position, 0.1, 0, 0, "X: %.2f", "Y: %.2f", "Z: %.2f")) change = true;
			if (Draw3Float("Scale:", &scale, 0.1, 0, 0, "X: %.2f", "Y: %.2f", "Z: %.2f")) change = true;
		}

		if (ImGui::CollapsingHeader("Config"))
		{
			DrawFloat("Reflection:", &reflectionFactor, 0.01, 0, 1, "%.2f");
			DrawFloat("Refraction:", &refractionFactor, 0.01, 0, 1, "%.2f");
			DrawFloat("Density   :", &densityFactor,    0.01, 0, 1, "%.2f");
		}

		return change;
	}

	float ReflectFactor() const
	{
		return reflectionFactor;
	}

	float RefractFactor() const
	{
		return refractionFactor;
	}

	void UpdateTransform() override
	{
		glm::mat4 pos = glm::translate(position);
		glm::mat4 rot = glm::rotate(glm::radians(rotation.x), glm::vec3(1, 0, 0));
		rot = glm::rotate(rot, glm::radians(rotation.y), glm::vec3(0, 1, 0));
		rot = glm::rotate(rot, glm::radians(rotation.z), glm::vec3(0, 0, 1));
		glm::mat4 scl = glm::scale(scale);

		plane->world = pos * rot * scl;
	}

private:

	WaterType type;
	glm::vec3 baseColor = glm::vec3(0, 0, 0.5);

	float refractionFactor = 0.7f;
	float reflectionFactor = 0.5f;
	float densityFactor = 0.5f;

public:

	WaterBuffer buffer;

	Model* plane = nullptr;

};