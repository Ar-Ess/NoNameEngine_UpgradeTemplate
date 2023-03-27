#pragma once

struct VertexBufferAttribute
{
	VertexBufferAttribute(unsigned char location, unsigned char componentCount, unsigned char offset)
	{
		this->location = location;
		this->componentCount = componentCount;
		this->offset = offset;
	}

	unsigned char location;
	unsigned char componentCount;
	unsigned char offset;
};
