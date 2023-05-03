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
	}

	ObjectType Type() const
	{
		return type;
	}

public:

	glm::mat4 world;
	GLuint vertexs;
	GLuint indexs;
	GLuint texUniform;
	GLuint program;
	bool draw = true;

protected:

	ObjectType type = ObjectType::O_EMPTY;

};
