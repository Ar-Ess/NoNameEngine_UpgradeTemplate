#pragma once
#include <glad/glad.h>
#include "Vao.h"
#include <imgui.h>

enum class ObjectType
{
	O_EMPTY,
	O_TEXTURED_QUAD,
	O_MODEL,
	O_LIGHT
};

class Object
{
public:

	Object(ObjectType type) : 
		type(type)
	{
		world = glm::mat4(1.0f);
		id = reinterpret_cast<intptr_t>(this);
		position = glm::vec3(0);
		rotation = glm::vec3(0);
		scale = glm::vec3(1);
	}

	ObjectType Type() const
	{
		return type;
	}

	virtual void UpdateTransform()
	{
		glm::mat4 pos = glm::translate(position);
		glm::mat4 rot = glm::rotate(glm::radians(rotation.x), glm::vec3(1, 0, 0));
		rot = glm::rotate(rot, glm::radians(rotation.y), glm::vec3(0, 1, 0));
		rot = glm::rotate(rot, glm::radians(rotation.z), glm::vec3(0, 0, 1));
		glm::mat4 scl = glm::scale(scale);

		world = pos * rot * scl;
	}

	virtual bool DrawGui()
	{
		return false;
	}

protected:

	bool DrawFloat(const char* title, float* value, float velocity, float min, float max, const char* format = "%.2f")
	{
		ImGui::PushItemWidth(70);
		std::string id = "##";
		id += title;
		ImGui::BulletText(title);
		ImGui::SameLine();
		bool change = ImGui::DragFloat(id.c_str(), value, velocity, min, max, format);

		ImGui::PopItemWidth();

		return change;
	}

	bool Draw3Float(const char* title, glm::vec3* value, float velocity, float min, float max, const char* format1, const char* format2, const char* format3)
	{
		ImGui::PushItemWidth(50);
		ImGui::BulletText(title);
		std::string id = "##";
		id += title;

		bool change = false;

		if (ImGui::DragFloat((id + "1").c_str(), &value->x, velocity, min, max, format1)) change = true;
		ImGui::SameLine();
		if (ImGui::DragFloat((id + "2").c_str(), &value->y, velocity, min, max, format2)) change = true;
		ImGui::SameLine();
		if (ImGui::DragFloat((id + "3").c_str(), &value->z, velocity, min, max, format3)) change = true;

		ImGui::PopItemWidth();

		return change;
	}

private:

	glm::vec3 InternalPosition() const
	{
		return glm::vec3(world[3].x, world[3].y, world[3].z);
	}

	glm::vec3 InternalScale() const
	{
		return glm::vec3(world[0].x, world[1].y, world[2].z);
	}

public:

	glm::mat4 world;
	u32 localParamsOffset = 0;
	u32 localParamsSize = 0;
	intptr_t id = 0;
	std::string name;

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	GLuint vertexs = 0;
	GLuint indexs = 0;
	GLuint texUniform = 0;
	GLuint program = 0;
	bool active = true;

protected:

	ObjectType type = ObjectType::O_EMPTY;

};
