#pragma once
#include "figure.h"

class Point : public Figure
{
public:
  float R;

  Point(glm::vec3 position = glm::vec3(0.f), float Rn = 0.05f, bool numerate = true);

  void Render(int colorLoc, int modelLoc, bool grayscale);
  bool GetBoundingSphere(CAD::Sphere &sphere);
  int Serialize(MG1::Scene &scene, std::vector<uint32_t> cpsIdxs) override;
  void CalculateModelMatrix() override;
  float GetR() { return R; }
  bool Intersectional() { return false; }

private:
  std::tuple<std::vector<GLfloat>, std::vector<GLuint>> Calculate() const;
  std::tuple<std::vector<GLfloat>, std::vector<GLuint>>
  InitializeAndCalculate(float R);
};