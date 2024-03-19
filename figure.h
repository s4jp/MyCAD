#pragma once
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "matrix.cpp"

#include <glm/glm.hpp>
#include <vector>
#include <tuple>

class Figure
{
private:
  glm::vec3 scale;
  glm::vec3 angle;
  glm::vec3 translation;

protected:
  VAO vao;
  VBO vbo;
  EBO ebo;

  glm::mat4 model;
  int indices_count;

public:

  virtual void Render(int colorLoc, int modelLoc, glm::mat4 modelMatrix) = 0;

  void Delete() {
    vao.Delete();
    vbo.Delete();
    ebo.Delete();
  };

  void CalculateModelMatrix() {
    model =
        translate(rotate(scaling(glm::mat4(1.0f), scale), angle), translation);
  };

  Figure(std::tuple<std::vector<GLfloat>, std::vector<GLuint>> data) {
    scale = glm::vec3(1.0f);
    angle = glm::vec3(0.0f);
    translation = glm::vec3(0.0f);
    indices_count = std::get<1>(data).size();
    model = glm::mat4(1.0f);

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

  glm::vec3 GetScale() { return scale; }
  glm::vec3 GetAngle() { return angle; }
  glm::vec3 GetTranslation() { return translation; }

  void SetScale(glm::vec3 nScale) {
    scale = nScale;
    CalculateModelMatrix();
  }
  void SetAngle(glm::vec3 nAngle) {
    angle = nAngle;
    CalculateModelMatrix();
  }
  void SetTranslation(glm::vec3 nTranslation) {
    translation = nTranslation;
    CalculateModelMatrix();
  }
};