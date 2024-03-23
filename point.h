#pragma once
#include "figure.h"

class Point : public Figure
{
public:
  float R;

  Point(float Rn = 0.05f);

  void Render(int colorLoc, int modelLoc);
  void CreateImgui(){};
  bool GetBoundingSphere(CAD::Sphere &sphere);

private:
  std::tuple<std::vector<GLfloat>, std::vector<GLuint>> Calculate();
  std::tuple<std::vector<GLfloat>, std::vector<GLuint>>
  InitializeAndCalculate(float R);
};