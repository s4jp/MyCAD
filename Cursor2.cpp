#include "Cursor2.h"

#include <glm/gtc/type_ptr.hpp>

std::string const Cursor2::GetClassName()
{
	return "Cursor";
}

Cursor2::Cursor2(float length, glm::vec3 position, GLint modelLoc, GLint colorLoc, bool numerate) 
	: length(length), SimpleFigure(position, modelLoc, GetClassName(), colorLoc, numerate) {
	Calculate(false);
}

void Cursor2::Calculate(bool recalculate)
{
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;

	vertices.push_back(0.f);
	vertices.push_back(0.f);
	vertices.push_back(0.f);

	vertices.push_back(length);
	vertices.push_back(0.f);
	vertices.push_back(0.f);

	vertices.push_back(0.f);
	vertices.push_back(length);
	vertices.push_back(0.f);

	vertices.push_back(0.f);
	vertices.push_back(0.f);
	vertices.push_back(length);

	indices.push_back(0);
	indices.push_back(1);

	indices.push_back(0);
	indices.push_back(2);

	indices.push_back(0);
	indices.push_back(3);

	InitAndFillBuffers(vertices, indices);
}

void Cursor2::Render()
{
	vao.Bind();
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glLineWidth(5.0f);

	glUniform4fv(colorLoc, 1, glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);

	glUniform4fv(colorLoc, 1, glm::value_ptr(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)));
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(2 * sizeof(GLuint)));

	glUniform4fv(colorLoc, 1, glm::value_ptr(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(4 * sizeof(GLuint)));

	vao.Unbind();
}
