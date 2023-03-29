#pragma once
#include "Object.h"

class TexturedQuad : public Object
{
public:

	TexturedQuad() : Object(
		ObjectType::O_TEXTURED_QUAD
	) {}

public:

	Vao vao;
	GLuint texture;

};
