#pragma once
#include <vector>
#include "VertexBufferAttibute.h"

struct VertexBufferLayout
{
	std::vector<VertexBufferAttribute*> attributes;
	unsigned char stride;

	template <typename T>
	void AddAttribute(VertexBufferAttribute* attribute)
	{
		attributes.push_back(attribute);
		attribute->offset = stride;
		stride += attribute->componentCount * sizeof(T);
	}

	void Bound()
	{
		if (stride % 4 == 0) return;
		double times = ceil(stride / 4);
		stride = int(times * 4);
	}

};