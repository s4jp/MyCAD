#include "bezierC2.h"

BezierC2::BezierC2(int cpCountLoc, int segmentCountLoc, int segmentIdxLoc)
    : BezierC0(cpCountLoc, segmentCountLoc, segmentIdxLoc, "Bezier C2") {
  bSpline =
      new BezierC0(cpCountLoc, segmentCountLoc, segmentIdxLoc, "B-spline");
}

std::tuple<std::vector<GLfloat>, std::vector<GLuint>>
BezierC2::Calculate() const { 
  CalculateBspline();
  return BezierC0::Calculate();
}

void BezierC2::CalculateBspline() const {
  // actual conversion
  int k = controlPoints.size();

  std::vector<glm::vec3> g;
  std::vector<glm::vec3> f;
  std::vector<glm::vec3> e;

  for (int i = 0; i < k - 2; i++) {
    g.push_back(1.f / 3 * controlPoints[i]->GetPosition() +
                2.f / 3 * controlPoints[i + 1]->GetPosition());
    f.push_back(2.f / 3 * controlPoints[i + 1]->GetPosition() +
                1.f / 3 * controlPoints[i + 2]->GetPosition());
    e.push_back(0.5f * g[i] + 0.5f * f[i]);
  }

  std::vector<glm::vec3> nCPs;

  for (int i = 0; i < k - 3; i++) {
    nCPs.push_back(e[i]);
    nCPs.push_back(f[i]);
    nCPs.push_back(g[i + 1]);
  }
  if (k - 3 > 0) {
    nCPs.push_back(e[k - 3]);
  }

  // sending data to bSpline
  bSpline->ClearControlPoints();
  for (int i = 0; i < nCPs.size(); i++) {
    bSpline->AddControlPoint(new Point(nCPs[i],0.02F));
  }
}

void BezierC2::Render(int colorLoc, int modelLoc) {
  bSpline->Render(colorLoc, modelLoc);
}

void BezierC2::RenderPolyline(int colorLoc, int modelLoc) {
  BezierC0::RenderPolyline(colorLoc, modelLoc);

  bSpline->RenderPolyline(colorLoc, modelLoc);
  std::vector<Figure *> cp = bSpline->GetControlPoints();
  for (int i = 0; i < cp.size(); i++) {
    cp[i]->Render(colorLoc, modelLoc);
  }
}
