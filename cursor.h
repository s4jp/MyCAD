#pragma once
#include "figure.h"

class Cursor : public Figure 
{
public:
  float length;

  Cursor(glm::vec3 position = glm::vec3(0.f), float lengthN = 0.05f);

  void Render(int colorLoc, int modelLoc);
  void CreateImgui(){};
  bool GetBoundingSphere(CAD::Sphere &sphere) { return false; };

private:
  std::tuple<std::vector<GLfloat>, std::vector<GLuint>> Calculate();
  std::tuple<std::vector<GLfloat>, std::vector<GLuint>>
  InitializeAndCalculate(float length);
};