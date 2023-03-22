#pragma once
#include <glad/glad.h>

enum class MeshType
{
	M_EMPTY,
	M_TEXTURED_QUAD,
	M_MODEL
};

class Mesh
{
public:

	Mesh(MeshType type) : 
		type(type)
	{}

	MeshType Type() const
	{
		return type;
	}

public:

	GLuint vertex;
	GLuint indexs;
	GLuint vao;
	u32 program;
	GLuint uniform;
	GLuint texture;
	bool draw = true;

protected:

	MeshType type = MeshType::M_EMPTY;

};
