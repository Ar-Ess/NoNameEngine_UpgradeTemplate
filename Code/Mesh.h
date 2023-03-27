#pragma once

#include "VertexBufferLayout.h"

struct Mesh
{
	VertexBufferLayout vertexBufferLayout;
	std::vector<float> vertexs;
	std::vector<unsigned int> indexs;
	unsigned int vertexOffset;
	unsigned int indexsOffset;
	std::vector<Vao> vaos;
};