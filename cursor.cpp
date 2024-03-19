#include "cursor.h"
#include <glm/gtc/type_ptr.hpp>

Cursor::Cursor(float lengthN) : Figure(InitializeAndCalculate(lengthN)) {}

void Cursor::Render(int colorLoc, int modelLoc, glm::mat4 modelMatrix) 
{ 
	vao.Bind();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix * model));
	glLineWidth(5.0f);

    glUniform4fv(colorLoc, 1, glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);

	glUniform4fv(colorLoc, 1, glm::value_ptr(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)));
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(2 * sizeof(GLuint)));

	glUniform4fv(colorLoc, 1, glm::value_ptr(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(4 * sizeof(GLuint)));

	vao.Unbind();
}

std::tuple<std::vector<GLfloat>, std::vector<GLuint>> Cursor::Calculate() {
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

  return std::make_tuple(vertices, indices);
}

std::tuple<std::vector<GLfloat>, std::vector<GLuint>>
Cursor::InitializeAndCalculate(float length) {
  this->length = length;

  return Calculate();
}
