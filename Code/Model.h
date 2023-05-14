#pragma once
#include "Object.h"
#include "Mesh.h"
#include "VertexBufferLayout.h"
#include <vector>
#include "Program.h"
#include "ShaderMaterial.h"

class Model : public Object
{
public:

	Model() : Object(
		ObjectType::O_MODEL
	) {}

	bool DrawGui() override
	{
		bool change = false;

		ImGui::PushItemWidth(50);

		ImGui::BulletText("Position:");
		if (ImGui::DragFloat("##1x", &position.x, 0.1, 0, 0, "X: %.2f")) change = true;
		ImGui::SameLine();
		if (ImGui::DragFloat("##1y", &position.y, 0.1, 0, 0, "Y: %.2f")) change = true;
		ImGui::SameLine();
		if (ImGui::DragFloat("##1z", &position.z, 0.1, 0, 0, "Z: %.2f")) change = true;

		ImGui::BulletText("Rotation:");
		if (ImGui::DragFloat("##2x", &rotation.x, 0.1, 0, 0, "X: %.2f")) change = true;
		ImGui::SameLine();
		if (ImGui::DragFloat("##2y", &rotation.y, 0.1, 0, 0, "Y: %.2f")) change = true;
		ImGui::SameLine();
		if (ImGui::DragFloat("##2z", &rotation.z, 0.1, 0, 0, "Z: %.2f")) change = true;

		ImGui::BulletText("Scale:");
		if (ImGui::DragFloat("##3x", &scale.x, 0.1, 0, 0, "X: %.2f")) change = true;
		ImGui::SameLine();
		if (ImGui::DragFloat("##3y", &scale.y, 0.1, 0, 0, "Y: %.2f")) change = true;
		ImGui::SameLine();
		if (ImGui::DragFloat("##3z", &scale.z, 0.1, 0, 0, "Z: %.2f")) change = true;

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

		glBindBuffer(GL_ARRAY_BUFFER, vertexs);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexs);

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

	ShaderMaterial material;
	std::vector<Mesh*> meshes;
	std::vector<unsigned int> materials;

};