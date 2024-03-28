#pragma once
#define _USE_MATH_DEFINES
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "helpers.h"

#include <glm/glm.hpp>
#include <tuple>
#include <string>
#include"imgui.h"

class Figure
{
private:
  glm::vec3 scale;
  glm::vec3 angle;
  glm::vec3 translation;
  glm::vec3 center;

  static int counter;

  glm::vec3 cScale;
  glm::vec3 cAngle;
  glm::vec3 cTranslation;

protected:
  VAO vao;
  VBO vbo;
  EBO ebo;

  glm::mat4 model;
  size_t indices_count;
  bool is3D = false;

  glm::vec4 GetColor() const {
    return selected ? glm::vec4(0.89f, 0.29f, 0.15f, 1.0f)
                    : glm::vec4(1.0f, 0.73f, 0.31f, 1.0f);
  };

public:
  std::string name;
  bool selected = false;

  virtual void Render(int colorLoc, int modelLoc) = 0;
  virtual bool GetBoundingSphere(CAD::Sphere& sphere) = 0;
  virtual void CreateImgui(){
    ImGui::SeparatorText((name + " options:").c_str());

    // display position
    glm::vec3 pos = GetPosition();
    ImGui::Text(("Position: \nX:" + std::to_string(pos.x) + ", Y:" +
                 std::to_string(pos.y) + ", Z:" + std::to_string(pos.z))
                    .c_str());

    // translation manipulation
    ImGui::SeparatorText("Translation");
    if (ImGui::InputFloat("X", &translation.x, 0.01f, 1.f, "%.2f"))
        CalculateModelMatrix();
    if (ImGui::InputFloat("Y", &translation.y, 0.01f, 1.f, "%.2f"))
      CalculateModelMatrix();
    if (ImGui::InputFloat("Z", &translation.z, 0.01f, 1.f, "%.2f"))
      CalculateModelMatrix();
    if (ImGui::Button("Reset translation"))
      SetTranslation(glm::vec3(0.0f));

    if (!is3D)
      return;

    // scaling manipulation
    ImGui::SeparatorText("Scale");
    if (ImGui::InputFloat("Sx", &scale.x, 0.01f, 1.f, "%.2f"))
      CalculateModelMatrix();
    if (ImGui::InputFloat("Sy", &scale.y, 0.01f, 1.f, "%.2f"))
      CalculateModelMatrix();
    if (ImGui::InputFloat("Sz", &scale.z, 0.01f, 1.f, "%.2f"))
      CalculateModelMatrix();
    if (ImGui::Button("Reset scale"))
      SetScale(glm::vec3(1.0f));

    // rotation manipulation
    ImGui::SeparatorText("Rotation");
    if (ImGui::SliderAngle("X axis", &angle.x, -180.f, 180.f))
      CalculateModelMatrix();
    if (ImGui::SliderAngle("Y axis", &angle.y, -180.f, 180.f))
      CalculateModelMatrix();
    if (ImGui::SliderAngle("Z axis", &angle.z, -180.f, 180.f))
      CalculateModelMatrix();
    if (ImGui::Button("Reset rotation"))
      SetAngle(glm::vec3(0.0f));
  }

  void Delete() {
    vao.Delete();
    vbo.Delete();
    ebo.Delete();
  }
  void CalculateModelMatrix() {
    glm::mat4 translateM =
        CAD::translate(glm::mat4(1.0f), GetPosition());
    glm::mat4 rotateM = CAD::rotate(glm::mat4(1.0f), GetAngle());
    glm::mat4 scaleM = CAD::scaling(glm::mat4(1.0f), GetScale());

    model = translateM * rotateM * scaleM;
  }

  Figure(std::tuple<std::vector<GLfloat>, std::vector<GLuint>> data, std::string type, glm::vec3 center, bool numerate = false) {
    scale = glm::vec3(1.0f);
    angle = glm::vec3(0.0f);
    translation = glm::vec3(0.0f);
    this->center = center;
    indices_count = std::get<1>(data).size();
    model = glm::mat4(1.0f);

    cScale = glm::vec3(1.0f);
    cAngle = glm::vec3(0.0f);
    cTranslation = glm::vec3(0.0f);

    vao.Bind();
    vbo = VBO(std::get<0>(data).data(),
              std::get<0>(data).size() * sizeof(GLfloat));
    ebo =
        EBO(std::get<1>(data).data(), std::get<1>(data).size() * sizeof(GLint));

    vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, 0, (void *)0);
    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();

    name = type;
    if (numerate)
      name = name + " #" + std::to_string(++counter);

    CalculateModelMatrix();
  }

  glm::vec3 GetScale() const { return scale + cScale - glm::vec3(1.f); }
  glm::vec3 GetAngle() const { return angle + cAngle; }
  glm::vec3 GetPosition() const { return center + translation + cTranslation; }

  void SetScale(glm::vec3 nScale) {
    scale = nScale;
    CalculateModelMatrix();
  }
  void SetAngle(glm::vec3 nAngle) {
    angle = nAngle;

    while (angle.x > M_PI)
      angle.x -= M_PI * 2;
    while (angle.x < -M_PI)
      angle.x += M_PI * 2;

    while (angle.y > M_PI)
      angle.y -= M_PI * 2;
    while (angle.y < -M_PI)
      angle.y += M_PI * 2;

    while (angle.z > M_PI)
      angle.z -= M_PI * 2;
    while (angle.z < -M_PI)
      angle.z += M_PI * 2;

    CalculateModelMatrix();
  }
  void SetTranslation(glm::vec3 nTranslation) {
    translation = nTranslation;
    CalculateModelMatrix();
  }
  void SetPosition(glm::vec3 nPosition) {
    center = nPosition;
    translation = glm::vec3(0.f);
    CalculateModelMatrix();
  }
  void CalculatePivotTransformation(glm::vec3 centerPosition, glm::vec3 centerScale,
                       glm::vec3 centerAngle) { 

    glm::vec3 pos = center + translation;

      cTranslation.x =
          -centerPosition.z * centerScale.x *
              (cos(centerAngle.x) * sin(centerAngle.y) * cos(centerAngle.z) +
               sin(centerAngle.x) * sin(centerAngle.z)) -
          centerPosition.y * centerScale.x *
              (sin(centerAngle.x) * sin(centerAngle.y) * cos(centerAngle.z) -
               cos(centerAngle.x) * sin(centerAngle.z)) +
          pos.z * centerScale.x *
              (cos(centerAngle.x) * sin(centerAngle.y) * cos(centerAngle.z) +
               sin(centerAngle.x) * sin(centerAngle.z)) +
          pos.y * centerScale.x *
              (sin(centerAngle.x) * sin(centerAngle.y) * cos(centerAngle.z) -
               cos(centerAngle.x) * sin(centerAngle.z)) -
          cos(centerAngle.y) * centerPosition.x * centerScale.x *
              cos(centerAngle.z) +
          pos.x * cos(centerAngle.y) * centerScale.x * cos(centerAngle.z) +
          centerPosition.x - pos.x;

    cTranslation.y =
        -centerPosition.z * centerScale.y *
            (cos(centerAngle.x) * sin(centerAngle.y) * sin(centerAngle.z) -
             sin(centerAngle.x) * cos(centerAngle.z)) -
        centerPosition.y * centerScale.y *
            (sin(centerAngle.x) * sin(centerAngle.y) * sin(centerAngle.z) +
             cos(centerAngle.x) * cos(centerAngle.z)) +
        pos.z * centerScale.y *
            (cos(centerAngle.x) * sin(centerAngle.y) * sin(centerAngle.z) -
             sin(centerAngle.x) * cos(centerAngle.z)) +
        pos.y * centerScale.y *
            (sin(centerAngle.x) * sin(centerAngle.y) * sin(centerAngle.z) +
             cos(centerAngle.x) * cos(centerAngle.z)) -
        cos(centerAngle.y) * centerPosition.x * centerScale.y *
            sin(centerAngle.z) +
        pos.x * cos(centerAngle.y) * centerScale.y * sin(centerAngle.z) +
        centerPosition.y - pos.y;

      cTranslation.z =
          -sin(centerAngle.x) * cos(centerAngle.y) * centerPosition.y *
              centerScale.z -
          cos(centerAngle.x) * cos(centerAngle.y) * centerPosition.z *
              centerScale.z +
          pos.y * sin(centerAngle.x) * cos(centerAngle.y) * centerScale.z +
          pos.z * cos(centerAngle.x) * cos(centerAngle.y) * centerScale.z +
          sin(centerAngle.y) * centerPosition.x * centerScale.z -
          pos.x * sin(centerAngle.y) * centerScale.z + centerPosition.z - pos.z;

      cAngle = centerAngle;
      cScale = centerScale;

      CalculateModelMatrix();
  }
  void SavePivotTransformations() {
    SetTranslation(translation + cTranslation);
    cTranslation = glm::vec3(0.f);

    SetAngle(angle + cAngle);
    cAngle = glm::vec3(0.f);

    SetScale(scale + cScale - glm::vec3(1.f));
    cScale = glm::vec3(1.f);

    CalculateModelMatrix();
  }
};