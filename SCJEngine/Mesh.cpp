#include "pch.h"
#include "Mesh.h"

Mesh::Mesh()
{
	vertices = {
		{-1,-1,1},
		{1,-1,1},
		{-1,1,1},
		{1,1,1},
		{-1,-1,-1},
		{1,-1,-1},
		{-1,1,-1},
		{1,1,-1},
	};
	triangles = {
		{0,1,3},
		{0,3,2},
		{4,7,5},
		{4,6,7},

		{0,6,4},
		{0,2,6},
		{1,5,7},
		{1,7,3},

		{0,5,1},
		{0,4,5},
		{2,3,7},
		{2,7,6},
	};
}

void MeshRenderer::start()
{
	auto mesh = _object->getComponent<Mesh>();
	if (mesh) {
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(vec3), &mesh->vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->triangles.size() * sizeof(ivec3), &mesh->triangles[0], GL_STATIC_DRAW);
	}
}


void MeshRenderer::render(const mat4& view, const mat4& proj)
{
	shader.use();
	auto mesh = _object->getComponent<Mesh>();
	auto model=_object->getWorldTransform();
	auto view_inv = inverse(view);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, (GLfloat*)&model);
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, (GLfloat*)&view);
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "camera_transform"), 1, GL_FALSE, (GLfloat*)&view_inv);
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, (GLfloat*)&proj);
	glDrawElements(GL_TRIANGLES, mesh->triangles.size() * 3, GL_UNSIGNED_INT, 0);
}

MeshRenderer::MeshRenderer()
{
	shader=shader_asset["mesh"];

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);


	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
}
