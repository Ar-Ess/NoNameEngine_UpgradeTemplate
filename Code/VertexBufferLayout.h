#pragma once
#include <vector>
#include "VertexBufferAttibute.h"

struct VertexBufferLayout
{
	std::vector<VertexBufferAttribute*> attributes;
	unsigned char stride;
};