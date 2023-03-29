#pragma once

struct Vao
{
	Vao() {}

	Vao(unsigned int handle, unsigned int program)
	{
		this->handle = handle;
		this->program = program;
	}
	unsigned int handle = 0;
	unsigned int program = 0;
};