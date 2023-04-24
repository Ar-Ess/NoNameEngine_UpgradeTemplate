#pragma once

struct VertexBufferAttribute
{
	VertexBufferAttribute(unsigned char location, unsigned char componentCount)
	{
		this->location = location;
		this->componentCount = componentCount;
		this->offset = 0;
	}

	unsigned char location;
	unsigned char componentCount;
	unsigned char offset;
};
