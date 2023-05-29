#pragma once
#include "Object.h"

class TexturedQuad : public Object
{
public:

	TexturedQuad() : Object(
		ObjectType::O_TEXTURED_QUAD
	) {}

	bool DrawGui() override
	{
		bool change = false;

		ImGui::PushItemWidth(50);

		ImGui::BulletText("Position:");
		if (ImGui::DragFloat("##1x", &position.x, 0.1, 0, 0, "X: %.2f")) change = true;
		ImGui::SameLine();
		if (ImGui::DragFloat("##1y", &position.y, 0.1, 0, 0, "Y: %.2f")) change = true;
		ImGui::SameLine();
		if (ImGui::DragFloat("##1z", &position.z, 0.1, 0, 0, "Z: %.2f")) change = true;

		ImGui::BulletText("Rotation:");
		if (ImGui::DragFloat("##2x", &rotation.x, 0.1, 0, 0, "X: %.2f")) change = true;
		ImGui::SameLine();
		if (ImGui::DragFloat("##2y", &rotation.y, 0.1, 0, 0, "Y: %.2f")) change = true;
		ImGui::SameLine();
		if (ImGui::DragFloat("##2z", &rotation.z, 0.1, 0, 0, "Z: %.2f")) change = true;

		ImGui::BulletText("Scale:");
		if (ImGui::DragFloat("##3x", &scale.x, 0.1, 0, 0, "X: %.2f")) change = true;
		ImGui::SameLine();
		if (ImGui::DragFloat("##3y", &scale.y, 0.1, 0, 0, "Y: %.2f")) change = true;
		ImGui::SameLine();
		if (ImGui::DragFloat("##3z", &scale.z, 0.1, 0, 0, "Z: %.2f")) change = true;

		ImGui::PopItemWidth();

		return change;
	}

public:

	Vao vao;
	GLuint texture;

	GLuint lightingPassProgram;
	GLuint textureProgram;
	GLuint bloomProgram;

	GLuint textureProgramUniform;

};
