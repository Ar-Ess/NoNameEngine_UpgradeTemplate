#pragma once
#include <glad/glad.h>
#include "Vao.h"

enum class ObjectType
{
	O_EMPTY,
	O_TEXTURED_QUAD,
	O_MODEL
};

class Object
{
public:

	Object(ObjectType type) : 
		type(type)
	{
		world = glm::mat4(1.0f);
		id = reinterpret_cast<intptr_t>(this);
	}

	ObjectType Type() const
	{
		return type;
	}

	void UpdateTransform()
	{
		world = glm::translate(glm::mat4(1.0f), position);
		world = glm::rotate(world, glm::radians(rotation.x), glm::vec3(1, 0, 0));
		world = glm::rotate(world, glm::radians(rotation.y), glm::vec3(0, 1, 0));
		world = glm::rotate(world, glm::radians(rotation.z), glm::vec3(0, 0, 1));
	}

public:

	glm::mat4 world;
	u32 localParamsOffset;
	u32 localParamsSize;
	intptr_t id = 0;
	const char* name = nullptr;

	glm::vec3 position;
	glm::vec3 rotation;

	GLuint vertexs;
	GLuint indexs;
	GLuint texUniform;
	GLuint program;
	bool draw = true;

protected:

	ObjectType type = ObjectType::O_EMPTY;

};
