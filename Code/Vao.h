#pragma once

struct Vao
{
	Vao() {}

	Vao(unsigned int handle, unsigned int program, bool forward)
	{
		this->handle = handle;
		forward ? this->programFW = programFW : this->programDF = programDF;
	}

	unsigned int handle = 0;
	unsigned int programFW = 0;
	unsigned int programDF = 0;
};