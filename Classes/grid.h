#pragma once
#include "figure.h"

class Grid : public Figure
{
public:
  float size;
  int division;

  Grid(glm::vec3 position = glm::vec3(0.f), float sizeN = 30.f,
       int divisionN = 50);

  void Render(int colorLoc, int modelLoc, bool grayscale = false);
  bool GetBoundingSphere(CAD::Sphere& sphere) { return false; };

private:
  std::tuple<std::vector<GLfloat>, std::vector<GLuint>> Calculate() const;
  std::tuple<std::vector<GLfloat>, std::vector<GLuint>>
  InitializeAndCalculate(float size, int division);
};