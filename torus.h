#pragma once
#include "figure.h"

class Torus : public Figure 
{
public:
  float R1;
  float R2;
  int n1;
  int n2;

  Torus(float R1, float R2, int n1, int n2);

  void Recalculate();
  void Render(int colorLoc, int modelLoc, glm::mat4 modelMatrix);

private:
  std::tuple<std::vector<GLfloat>, std::vector<GLuint>> Calculate();
  std::tuple<std::vector<GLfloat>, std::vector<GLuint>>
  InitializeAndCalculate(float R1, float R2, int n1, int n2);
};