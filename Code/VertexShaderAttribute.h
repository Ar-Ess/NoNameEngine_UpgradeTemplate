#pragma once

struct VertexShaderAttribute
{
	VertexShaderAttribute(unsigned char location, unsigned char componentCount)
	{
		this->location = location;
		this->componentCount = componentCount;
	}
	unsigned char location;
	unsigned char componentCount;
};