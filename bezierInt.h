#pragma once
#include "bezierC0.h"

class BezierInt : public BezierC0 {
private:
  BezierC0 *bSpline;

  std::tuple<std::vector<GLfloat>, std::vector<GLuint>>
  Calculate() const override;
  void CalculateBspline() const;

public:
  BezierInt(int cpCountLoc, int segmentCountLoc, int segmentIdxLoc);

  void Render(int colorLoc, int modelLoc) override;
};