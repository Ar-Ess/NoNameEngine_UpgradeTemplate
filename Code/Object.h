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
	{}

	ObjectType Type() const
	{
		return type;
	}

public:

	GLuint vertexs;
	GLuint indexs;
	GLuint texUniform;
	GLuint program;
	bool draw = true;

protected:

	ObjectType type = ObjectType::O_EMPTY;

};
