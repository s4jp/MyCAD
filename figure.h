#pragma once
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

#include <glm/glm.hpp>
#include <vector>
#include <tuple>

class Figure
{
public:
  VAO vao;
  VBO vbo;
  EBO ebo;

  glm::vec3 scale;
  glm::vec3 angle;
  glm::vec3 translation;

  int indices_count;

  virtual void Render() = 0;
  virtual void Recalculate() = 0;

  void Delete() {
    vao.Delete();
    vbo.Delete();
    ebo.Delete();
  };

  Figure(std::tuple<std::vector<GLfloat>, std::vector<GLuint>> data) {
    scale = glm::vec3(1.0f);
    angle = glm::vec3(0.0f);
    translation = glm::vec3(0.0f);
    indices_count = std::get<1>(data).size();

    vao.Bind();
    vbo = VBO(std::get<0>(data).data(),
              std::get<0>(data).size() * sizeof(GLfloat));
    ebo =
        EBO(std::get<1>(data).data(), std::get<1>(data).size() * sizeof(GLint));

    vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, 0, (void *)0);
    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();
  }
};