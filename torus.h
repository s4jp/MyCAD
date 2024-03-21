#pragma once
#include "figure.h"

class Torus : public Figure 
{
public:
  float R1;
  float R2;
  int n1;
  int n2;

  Torus(float R1 = 0.5f, float R2 = 0.2f, int n1 = 10, int n2 = 20);

  void Recalculate();
  void Render(int colorLoc, int modelLoc);
  void CreateImgui();

private:
  std::tuple<std::vector<GLfloat>, std::vector<GLuint>> Calculate() const;
  std::tuple<std::vector<GLfloat>, std::vector<GLuint>>
  InitializeAndCalculate(float R1, float R2, int n1, int n2);
};