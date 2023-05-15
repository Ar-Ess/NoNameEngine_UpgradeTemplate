#pragma once

enum class LightType
{
	LT_NONE,
	LT_DIRECTIONAL,
	LT_POINT,
	LT_SPOT
};

struct Light : public Object
{

	Light(LightType type, glm::vec3 color, glm::vec3 position, glm::vec3 direction, float cutoff) : 
		type(type), color(color), direction(glm::normalize(direction)), cutoff(cutoff), Object(ObjectType::O_LIGHT)
	{
		this->position = position;
	}

	bool DrawGui() override
	{
		bool change = false;

		DrawFloat("Intensity:", &intensity, 0.01, 0, 2);

		switch (type)
		{
		case LightType::LT_POINT:
			if (ImGui::CollapsingHeader("Transform"))
				change = Draw3Float("Position:", &position, 0.1, 0, 0, "X: %.2f", "Y: %.2f", "Z: %.2f");
			break;

		case LightType::LT_DIRECTIONAL:
			if (ImGui::CollapsingHeader("Transform"))
				Draw3Float("Direction:", &direction, 0.01, -1, 1, "X: %.2f", "Y: %.2f", "Z: %.2f");
			break;

		case LightType::LT_SPOT:
			DrawFloat("Cutoff:   ", &cutoff, 0.1, 0, 360);
			DrawFloat("Softness: ", &softness, 0.1, 0, 360);
			if (ImGui::CollapsingHeader("Transform"))
			{
				change = Draw3Float("Position:", &position, 0.1, 0, 0, "X: %.2f", "Y: %.2f", "Z: %.2f");
				Draw3Float("Direction:", &direction, 0.01, -1, 1, "X: %.2f", "Y: %.2f", "Z: %.2f");
			}
			break;
		}

		if (ImGui::CollapsingHeader("Material"))
		{
			Draw3Float("Color:", &color, 0.01, 0, 1, "R: %.2f", "G: %.2f", "B: %.2f");
		}

		return change;
	}

	float Cutoff() const
	{
		return glm::cos(glm::radians(cutoff));
	}

	float OuterCuttoff() const
	{
		float fix = glm::clamp(360 - cutoff - softness, -999999999.f, 0.f);
		return glm::cos(glm::radians(cutoff + softness + fix));
	}

	LightType type = LightType::LT_NONE;
	glm::vec3 color = glm::vec3(1, 1, 1);
	glm::vec3 direction;
	float intensity = 1;

private:

	float softness = 0;
	float cutoff = 0;

};