#pragma once

enum class LightType
{
	LT_NONE,
	LT_DIRECTIONAL,
	LT_POINT
};

struct Light : public Object
{

	Light(LightType type, glm::vec3 color, glm::vec3 position, glm::vec3 direction) : 
		type(type), color(color), direction(glm::normalize(direction)), Object(ObjectType::O_LIGHT)
	{
		this->position = position;
	}

	bool DrawGui() override
	{
		bool change = false;

		ImGui::PushItemWidth(50);

		switch (type)
		{
		case LightType::LT_POINT:
			ImGui::BulletText("Position:");
			if (ImGui::DragFloat("##1x", &position.x, 0.1, 0, 0, "X: %.2f")) change = true;
			ImGui::SameLine();
			if (ImGui::DragFloat("##1y", &position.y, 0.1, 0, 0, "Y: %.2f")) change = true;
			ImGui::SameLine();
			if (ImGui::DragFloat("##1z", &position.z, 0.1, 0, 0, "Z: %.2f")) change = true;
			break;

		case LightType::LT_DIRECTIONAL:
			ImGui::BulletText("Direction:");
			ImGui::DragFloat("##2x", &direction.x, 0.1, 0, 0, "X: %.2f");
			ImGui::SameLine();
			ImGui::DragFloat("##2y", &direction.y, 0.1, 0, 0, "Y: %.2f");
			ImGui::SameLine();
			ImGui::DragFloat("##2z", &direction.z, 0.1, 0, 0, "Z: %.2f");
			break;
		}

		ImGui::BulletText("Color:");
		ImGui::DragFloat("##r", &color.x, 0.01, 0, 1, "R: %.2f");
		ImGui::SameLine();
		ImGui::DragFloat("##g", &color.y, 0.01, 0, 1, "G: %.2f");
		ImGui::SameLine();
		ImGui::DragFloat("##b", &color.z, 0.01, 0, 1, "B: %.2f");

		ImGui::PopItemWidth();

		return change;
	}

	LightType type = LightType::LT_NONE;
	glm::vec3 color = glm::vec3(1, 1, 1);
	glm::vec3 direction;

};