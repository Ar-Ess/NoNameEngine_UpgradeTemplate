#pragma once
#include "Object.h"
#include "Mesh.h"
#include "VertexBufferLayout.h"
#include <vector>
#include "Program.h"

class Model : public Object
{
public:

	Model() : Object(ObjectType::O_MODEL) {}

	bool DrawGui() override
	{
		bool change = false;

		ImGui::PushItemWidth(50);

		if (ImGui::CollapsingHeader("Transform"))
		{
			if (Draw3Float("Position:", &position, 0.1, 0, 0, "X: %.2f", "Y: %.2f", "Z: %.2f")) change = true;

			if (Draw3Float("Rotation:", &rotation, 0.1, 0, 0, "X: %.2f", "Y: %.2f", "Z: %.2f")) change = true;

			if (Draw3Float("Scale:", &scale, 0.1, 0, 0, "X: %.2f", "Y: %.2f", "Z: %.2f")) change = true;
		}

		ImGui::PopItemWidth();

		return change;
	}

	unsigned int FindVAO(unsigned int index, const Program* program)
	{
		Mesh* mesh = meshes[index];

		unsigned int size = mesh->vaos.size();
		for (unsigned int i = 0; i < size; ++i)
		{
			if (mesh->vaos[i].program == program->handle)
				return mesh->vaos[i].handle;
		}

		// Create new vao for this mesh/program
		unsigned int vaoHandle = CreateNewVao(program, mesh);

		// Store it in the list of vaos for this mesh
		mesh->vaos.emplace_back(Vao(vaoHandle, program->handle));

		return vaoHandle;
	}

	unsigned int CreateNewVao(const Program* program, Mesh* mesh)
	{
		GLuint vaoHandle = 0;
		glGenVertexArrays(1, &vaoHandle);
		glBindVertexArray(vaoHandle);

		glBindBuffer(GL_ARRAY_BUFFER, vertexHandle);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexHandle);

		for (VertexShaderLayout::const_iterator it = program->attributes.begin(); it != program->attributes.end(); ++it)
		{
			bool attributeWasLinked = false;

			for (std::vector<VertexBufferAttribute*>::const_iterator ot = mesh->vertexBufferLayout.attributes.begin(); ot != mesh->vertexBufferLayout.attributes.end(); ++ot)
			{
				if ((*it)->location != (*ot)->location) continue;

				const unsigned int index = (*ot)->location;
				const unsigned int ncomp = (*ot)->componentCount;
				const unsigned int offset = (*ot)->offset + mesh->vertexOffset;
				const unsigned int stride = mesh->vertexBufferLayout.stride;

				glVertexAttribPointer(index, ncomp, GL_FLOAT, GL_FALSE, stride, (void*)(u64)offset);
				glEnableVertexAttribArray(index);

				attributeWasLinked = true;
				break;
			}

			assert(attributeWasLinked);
		}

		glBindVertexArray(0);

		return vaoHandle;
	}

public:

	// Program list index, if handle needed, do this:
	// programs[this->forwardProgram]->handle
	GLuint forwardProgram = 0;

	// Program list index, if handle needed, do this:
	// programs[this->deferredProgram]->handle
	GLuint deferredProgram = 0;

	GLuint texUniformForward = 0;
	GLuint texUniformDeferred = 0;

	std::vector<Mesh*> meshes;
	std::vector<unsigned int> materials;

};