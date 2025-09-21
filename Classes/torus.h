#pragma once
#include "figure.h"

class Torus : public Figure 
{
private:
  float R1;
  float R2;
  int n1;
  int n2;

public:
  Torus(glm::vec3 position = glm::vec3(0.f), float R1 = 0.5f, float R2 = 0.2f,
        int n1 = 10,
        int n2 = 20);

  void Recalculate();
  void Render(int colorLoc, int modelLoc, bool grayscale);
  bool CreateImgui();
  bool GetBoundingSphere(CAD::Sphere &sphere) { return false; };
  int Serialize(MG1::Scene &scene, std::vector<uint32_t> cpsIdxs) override;

  bool Intersectional() { return true; }
  glm::vec3 GetValue(float u, float v) override;
  glm::vec3 GetTangentU(float u, float v) override;
  glm::vec3 GetTangentV(float u, float v) override;
  bool IsWrappedU() override { return true; }
  bool IsWrappedV() override { return true; }

private:
  std::tuple<std::vector<GLfloat>, std::vector<GLfloat>, std::vector<GLuint>> Calculate() const;
  std::tuple<std::vector<GLfloat>, std::vector<GLfloat>, std::vector<GLuint>>
  InitializeAndCalculate(float R1, float R2, int n1, int n2);
  float GetR() { return 0; };
};