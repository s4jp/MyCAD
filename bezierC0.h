#pragma once
#include "point.h"
#include "figure.h"

#include<vector>

class BezierC0 : public Figure
{
private:
  std::vector<Figure*> controlPoints;

  std::tuple<std::vector<GLfloat>, std::vector<GLuint>> Calculate() const;
  std::tuple<std::vector<GLfloat>, std::vector<GLuint>>
  InitializeAndCalculate(std::vector<Figure*> cps, int cpCountLoc);

public:
  int cpCountLoc;

  BezierC0(std::vector<Figure*> cps, int cpCountLoc);

  void Render(int colorLoc, int modelLoc);
  bool CreateImgui();
  bool GetBoundingSphere(CAD::Sphere &sphere) { return false; };

  std::vector<Figure*> GetControlPoints() { return controlPoints; };
  void AddControlPoint(Figure* cp);
  bool RemoveControlPoint(int idx);
  void RefreshBuffers();
  void RenderPolyline(int colorLoc, int modelLoc);
};