#include "bezierInt.h"

BezierInt::BezierInt(int cpCountLoc, int segmentCountLoc, int segmentIdxLoc)
    : BezierC0(cpCountLoc, segmentCountLoc, segmentIdxLoc, "Bezier Int") {
  bSpline = new BezierC0(cpCountLoc, segmentCountLoc, segmentIdxLoc, "B-spline",
                         false);
}

void BezierInt::Render(int colorLoc, int modelLoc) {
  bSpline->selected = selected;
  bSpline->Render(colorLoc, modelLoc);
}

void BezierInt::AddControlPoint(Figure *cp) {
  if (controlPoints.size() > 0) {
    float distance =
        glm::distance(controlPoints.back()->GetPosition(), cp->GetPosition());
    if (distance <= 1e-4)
      return;
  }
  BezierC0::AddControlPoint(cp);
}

std::tuple<std::vector<GLfloat>, std::vector<GLuint>>
BezierInt::Calculate() const {
  CalculateBspline();
  return BezierC0::Calculate();
}

void BezierInt::CalculateBspline() const { 
  bSpline->ClearControlPoints();
  
  const int N = (int)controlPoints.size() - 1;

  std::vector<float> dist;
  for (int i = 0; i < N; i++) {
    dist.push_back(glm::length(controlPoints[i + 1]->GetPosition() -
                            controlPoints[i]->GetPosition()));
  }

  std::vector<float> alpha, beta, diagonal;
  std::vector<glm::vec3> R;

  for (int i = 1; i < N; i++) {
    // alpha[0] and beta[N-1] won't be used
    alpha.push_back(dist[i - 1] / (dist[i - 1] + dist[i]));
    beta.push_back(dist[i] / (dist[i - 1] + dist[i]));
    R.push_back(3.f *
                ((controlPoints[i + 1]->GetPosition() -
                  controlPoints[i]->GetPosition()) /
                     dist[i] -
                 (controlPoints[i]->GetPosition() -
                  controlPoints[i - 1]->GetPosition()) /
                     dist[i - 1]) /
                (dist[i - 1] + dist[i]));
    diagonal.push_back(2.f);
  }

  std::vector<glm::vec3> c;
  c.push_back(glm::vec3(0.f));
  std::vector<glm::vec3> thomas =
      CAD::thomasAlgorihm(R, alpha, diagonal, beta);
  c.insert(c.end(), thomas.cbegin(), thomas.cend());
  c.push_back(glm::vec3(0.f));

  std::vector<glm::vec3> a;
  for (int i = 0; i < N + 1; i++) {
    a.push_back(controlPoints[i]->GetPosition());
  }

  std::vector<glm::vec3> d;
  for (int i = 1; i < N + 1; i++) {
    d.push_back((c[i] - c[i - 1]) / (3.f * dist[i - 1]));
  }

  std::vector<glm::vec3> b;
  for (int i = 1; i < N + 1; i++) {
    b.push_back((a[i] - a[i - 1] - c[i - 1] * dist[i - 1] * dist[i - 1] -
                 d[i - 1] * dist[i - 1] * dist[i - 1] * dist[i - 1]) /
                dist[i - 1]);
  }

  for (int i = 0; i < N; i++) {
    glm::mat3x4 temp = glm::mat3x4(0.f);
    temp[0] = glm::vec4(a[i].x, b[i].x, c[i].x, d[i].x);
    temp[1] = glm::vec4(a[i].y, b[i].y, c[i].y, d[i].y);
    temp[2] = glm::vec4(a[i].z, b[i].z, c[i].z, d[i].z);

    temp = CAD::powerToBerensteinBasis(temp, dist[i]);

    bSpline->AddControlPoint(
        new Point(glm::vec3(temp[0][0], temp[1][0], temp[2][0]), 0.05F, false));
    bSpline->AddControlPoint(
        new Point(glm::vec3(temp[0][1], temp[1][1], temp[2][1]), 0.05F, false));
    bSpline->AddControlPoint(
        new Point(glm::vec3(temp[0][2], temp[1][2], temp[2][2]), 0.05F, false));
    if (i == N - 1)
      bSpline->AddControlPoint(new Point(
          glm::vec3(temp[0][3], temp[1][3], temp[2][3]), 0.05F, false));
  }
}
