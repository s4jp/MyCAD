#pragma once
#include "point.h"
#include "figure.h"

#include<vector>

class BezierC0 : public Figure
{
private:
  std::tuple<std::vector<GLfloat>, std::vector<GLuint>>
  InitializeAndCalculate(int cpCountLoc, int segmentCountLoc,
                         int segmentIdxLoc);

protected:
  std::vector<Figure *> controlPoints;

  std::tuple<std::vector<GLfloat>, std::vector<GLuint>> virtual Calculate() const;

public:
  int cpCountLoc;
  int segmentCountLoc;
  int segmentIdxLoc;

  BezierC0(int cpCountLoc, int segmentCountLoc, int segmentIdxLoc,
           const char *name = "Bezier C0");

  void virtual Render(int colorLoc, int modelLoc);
  bool CreateImgui();
  bool GetBoundingSphere(CAD::Sphere &sphere) { return false; };

  std::vector<Figure*> GetControlPoints() { return controlPoints; };
  void AddControlPoint(Figure* cp);
  bool RemoveControlPoint(int idx);
  void RefreshBuffers();
  void virtual RenderPolyline(int colorLoc, int modelLoc);
  void ClearControlPoints();
};