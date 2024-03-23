#pragma once
#include "figure.h"

class Cursor : public Figure 
{
public:
  float length;

  Cursor(float lengthN);

  void Render(int colorLoc, int modelLoc);
  void CreateImgui(){};
  bool GetBoundingSphere(CAD::Sphere &sphere) { return false; };

private:
  std::tuple<std::vector<GLfloat>, std::vector<GLuint>> Calculate();
  std::tuple<std::vector<GLfloat>, std::vector<GLuint>>
  InitializeAndCalculate(float length);
};