#pragma once
#include "Object.h"
#include "Mesh.h"
#include "VertexBufferLayout.h"
#include "VertexShaderAttribute.h"
#include <vector>

typedef std::vector<VertexShaderAttribute*> VertexShaderLayout;

class Model : public Object
{
public:

	Model() : Object(
		ObjectType::O_MODEL
	) {}

private:

	std::vector<Mesh*> meshes;
	std::vector<unsigned int> materials;
	GLuint vertexBufferHandle;
	GLuint indexsBufferHandle;

};