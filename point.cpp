#include "point.h"
#include <glm/gtc/type_ptr.hpp>

Point::Point(float Rn) : Figure(InitializeAndCalculate(Rn)) {}

void Point::Render(int colorLoc, int modelLoc) 
{
  vao.Bind();
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

  glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, 0);

  vao.Unbind();
}

std::tuple<std::vector<GLfloat>, std::vector<GLuint>> Point::Calculate() {
  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;

  // bottom
  vertices.push_back(0.f);
  vertices.push_back(-R);
  vertices.push_back(0.f);
  // left
  vertices.push_back(R);
  vertices.push_back(0.f);
  vertices.push_back(0.f);
  // up
  vertices.push_back(0.f);
  vertices.push_back(0.f);
  vertices.push_back(-R);
  // right
  vertices.push_back(-R);
  vertices.push_back(0.f);
  vertices.push_back(0.f);
  // down
  vertices.push_back(0.f);
  vertices.push_back(0.f);
  vertices.push_back(R);
  // top
  vertices.push_back(0.f);
  vertices.push_back(R);
  vertices.push_back(0.f);


  indices.push_back(0);
  indices.push_back(1);
  indices.push_back(4);

  indices.push_back(0);
  indices.push_back(2);
  indices.push_back(1);

  indices.push_back(0);
  indices.push_back(3);
  indices.push_back(2);

  indices.push_back(0);
  indices.push_back(4);
  indices.push_back(3);

  indices.push_back(5);
  indices.push_back(4);
  indices.push_back(1);

  indices.push_back(5);
  indices.push_back(1);
  indices.push_back(2);

  indices.push_back(5);
  indices.push_back(2);
  indices.push_back(3);

  indices.push_back(5);
  indices.push_back(3);
  indices.push_back(4);

  return std::make_tuple(vertices, indices);
}

std::tuple<std::vector<GLfloat>, std::vector<GLuint>>
Point::InitializeAndCalculate(float R) {
  this->R = R;

  return Calculate();
};

