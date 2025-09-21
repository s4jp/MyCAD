#include "cursor.h"
#include <glm/gtc/type_ptr.hpp>

Cursor::Cursor(glm::vec3 position, float lengthN)
    : Figure(InitializeAndCalculate(lengthN), "Cursor", position) {}

void Cursor::Render(int colorLoc, int modelLoc, bool grayscale) 
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

bool Cursor::CreateImgui() {
  bool change = true;
  ImGui::SeparatorText((name + " options:").c_str());

  ImGui::Text("Position: ");
  glm::vec3 pos = GetPosition();
  if (ImGui::InputFloat("X ", &pos.x, 0.01f, 1.f, "%.5f")) {
    SetPosition(pos);
    change = true;
  }
  if (ImGui::InputFloat("Y ", &pos.y, 0.01f, 1.f, "%.5f")) {
    SetPosition(pos);
    change = true;
  }
  if (ImGui::InputFloat("Z ", &pos.z, 0.01f, 1.f, "%.5f")) {
    SetPosition(pos);
    change = true;
  }
  
  return change;
}

std::tuple<std::vector<GLfloat>, std::vector<GLfloat>, std::vector<GLuint>> Cursor::Calculate() {
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

  return std::make_tuple(vertices, std::vector<GLfloat>(), indices);
}

std::tuple<std::vector<GLfloat>, std::vector<GLfloat>, std::vector<GLuint>>
Cursor::InitializeAndCalculate(float length) {
  this->length = length;

  return Calculate();
}
