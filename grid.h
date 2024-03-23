#pragma once
#include "figure.h"

class Grid : public Figure
{
public:
  float size;
  int division;

  Grid(float sizeN, int divisionN);

  void Render(int colorLoc, int modelLoc);
  void CreateImgui(){};
  bool GetBoundingSphere(CAD::Sphere& sphere) { return false; };

private:
  std::tuple<std::vector<GLfloat>, std::vector<GLuint>> Calculate() const;
  std::tuple<std::vector<GLfloat>, std::vector<GLuint>>
  InitializeAndCalculate(float size, int division);
};