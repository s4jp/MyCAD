#define _USE_MATH_DEFINES
#include <tuple>
#include "torus.h"
#include <glm/gtc/type_ptr.hpp>

Torus::Torus(float R1n, float R2n, int n1n, int n2n)
    : Figure(InitializeAndCalculate(R1n,R2n,n1n,n2n)) {}

void Torus::Recalculate() 
{ 
  std::tuple<std::vector<GLfloat>, std::vector<GLuint>> data = Calculate();
  indices_count = std::get<1>(data).size();
  vbo.ReplaceBufferData(std::get<0>(data).data(),
                        std::get<0>(data).size() * sizeof(GLfloat));
  ebo.ReplaceBufferData(std::get<1>(data).data(),
                           std::get<1>(data).size() * sizeof(GLint));
}

void Torus::Render(int colorLoc) 
{
  glUniform4fv(colorLoc, 1,
               glm::value_ptr(glm::vec4(1.0f, 0.65f, 0.0f, 1.0f)));
  glLineWidth(3.0f);
  vao.Bind();
  glDrawElements(GL_LINES, indices_count, GL_UNSIGNED_INT, 0);
}

std::tuple<std::vector<GLfloat>, std::vector<GLuint>> Torus::Calculate() 
{
  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;

  float R1step = 2 * M_PI / n1;
  float R2step = 2 * M_PI / n2;

  for (int i = 0; i < n1; i++) {
    float xyElem = R1 + R2 * cos(i * R1step);
    float z = R2 * sin(i * R1step);

    for (int j = 0; j < n2; j++) {
      vertices.push_back(xyElem * cos(j * R2step)); // X
      vertices.push_back(xyElem * sin(j * R2step)); // Y
      vertices.push_back(z);                        // Z

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

