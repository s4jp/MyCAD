#pragma once
#include "point.h"
#include "figure.h"

#include<vector>

class BezierC0 : public Figure
{
private:
  std::vector<Point*> controlPoints;


  std::tuple<std::vector<GLfloat>, std::vector<GLuint>> Calculate() const;
  std::tuple<std::vector<GLfloat>, std::vector<GLuint>>
  InitializeAndCalculate(std::vector<Point*> cps);

public:
  BezierC0(glm::vec3 position, std::vector<Point*> cps);

  void Render(int colorLoc, int modelLoc);
  bool CreateImgui();
  bool GetBoundingSphere(CAD::Sphere &sphere) { return false; };

  std::vector<Point*> GetControlPoints() { return controlPoints; };
  void AddControlPoint(Point* cp);
  bool RemoveControlPoint(int idx);
  void RefreshBuffers();
};