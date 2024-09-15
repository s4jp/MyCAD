#define _USE_MATH_DEFINES
#include "torus.h"

#include <tuple>
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"

Torus::Torus(glm::vec3 position, float R1n, float R2n, int n1n, int n2n)
    : Figure(InitializeAndCalculate(R1n, R2n, n1n, n2n), "Torus", position,
             true) {
  is3D = true;
}

void Torus::Recalculate() 
{ 
  std::tuple<std::vector<GLfloat>, std::vector<GLuint>> data = Calculate();
  indices_count = std::get<1>(data).size();
  vbo.ReplaceBufferData(std::get<0>(data).data(),
                        std::get<0>(data).size() * sizeof(GLfloat));
  ebo.ReplaceBufferData(std::get<1>(data).data(),
                           std::get<1>(data).size() * sizeof(GLuint));
}

void Torus::Render(int colorLoc, int modelLoc, bool grayscale)
{
  vao.Bind();
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glLineWidth(3.0f);

  glUniform4fv(colorLoc, 1, glm::value_ptr(GetColor(grayscale)));
  glDrawElements(GL_LINES, indices_count, GL_UNSIGNED_INT, 0);

  vao.Unbind();
}

std::tuple<std::vector<GLfloat>, std::vector<GLuint>> Torus::Calculate() const 
{
  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;

  float R1step = 2 * M_PI / n1;
  float R2step = 2 * M_PI / n2;

  for (int i = 0; i < n1; i++) {
    float xyElem = R1 + R2 * cos(i * R1step);
    float z = R2 * sin(i * R1step);

    for (int j = 0; j < n2; j++) {
      // some work-around
      glm::vec4 vertex = glm::vec4(
          xyElem * cos(j * R2step),                 // X
          xyElem * sin(j * R2step),                 // Y
          z,                                        // Z
          1.0f);                                       
      vertex = CAD::rotate(glm::mat4(1.0f), glm::vec3(M_PI_2, 0.f, 0.f)) * vertex;
      vertices.push_back(vertex.x);
      vertices.push_back(vertex.y);
      vertices.push_back(vertex.z);

      // R2 loop
      indices.push_back(i * n2 + j);              // current
      indices.push_back(i * n2 + ((j + 1) % n2)); // next
      // R1 loop
      indices.push_back(i * n2 + j);              // current
      indices.push_back(((i + 1) % n1) * n2 + j); // next
    }
  }

  return std::make_tuple(vertices, indices);
}

std::tuple<std::vector<GLfloat>, std::vector<GLuint>>
Torus::InitializeAndCalculate(float R1, float R2, int n1, int n2) 
{
  this->R1 = R1;
  this->R2 = R2;
  this->n1 = n1;
  this->n2 = n2;

  return Calculate();
}

bool Torus::CreateImgui() 
{
    bool change = Figure::CreateImgui();

    ImGui::SeparatorText("Options");
    if (ImGui::SliderFloat("R1", &R1, 0.01f, 5.f, "%.2f")) {
      Recalculate();
      change = true;
    }
    if (ImGui::SliderFloat("R2", &R2, 0.01f, 5.f, "%.2f")) {
      Recalculate();
      change = true;
    }
    if (ImGui::SliderInt("major", &n2, 3, 50)) {
      Recalculate();
      change = true;
    }
    if (ImGui::SliderInt("minor", &n1, 3, 50)) {
      Recalculate();
      change = true;
    }

    return change;
}

int Torus::Serialize(MG1::Scene &scene, std::vector<uint32_t> cpsIdxs) 
{ 
  MG1::Torus t;
  t.position = CAD::serializeVec3(this->GetPosition());
  t.largeRadius = R1;
  t.smallRadius = R2;
  t.samples.x = n1;
  t.samples.y = n2;
  t.rotation = CAD::serializeVec3(this->GetAngle());
  t.scale = CAD::serializeVec3(this->GetScale());
  t.name = name;
  scene.tori.push_back(t);
  return -1;
}
