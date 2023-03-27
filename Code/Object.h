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

	GLuint vertex;
	GLuint indexs;

	Vao vao;
	GLuint uniform;
	GLuint texture;
	bool draw = true;

protected:

	ObjectType type = ObjectType::O_EMPTY;

};
