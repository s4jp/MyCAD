#include "bezierC2.h"

BezierC2::BezierC2(int cpCountLoc, int segmentCountLoc, int segmentIdxLoc, int divisionLoc)
    : BezierInt(cpCountLoc, segmentCountLoc, segmentIdxLoc, divisionLoc, "Bezier C2") {}

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
    Point* cp = new Point(nCPs[i], 0.02F, false);
    int mod = i % 3;
    int idx = glm::floor(i / 3.f);
    switch (mod) { 
      case 0:
      cp->name = "e" + std::to_string(idx);
        break;
      case 1:
        cp->name = "f" + std::to_string(idx);
        break;
      case 2:
        cp->name = "g" + std::to_string(idx + 1);
        break;
    }
    bSpline->AddControlPoint(cp);
  }
}

void BezierC2::RenderPolyline(int colorLoc, int modelLoc, bool grayscale) {
  if (!berensteinPolyline) {
    BezierC0::RenderPolyline(colorLoc, modelLoc, grayscale);
    return;
  }
  
  bSpline->RenderPolyline(colorLoc, modelLoc, grayscale);
  std::vector<Figure *> cp = bSpline->GetControlPoints();
  for (int i = 0; i < cp.size(); i++) {
    cp[i]->Render(colorLoc, modelLoc, grayscale);
  }
}

bool BezierC2::CreateImgui() { 
    ImGui::Checkbox("View Berenstein", &berensteinPolyline);

    return BezierC0::CreateImgui();
}

void BezierC2::CreateBsplineImgui() { 
  this->berensteinPolyline = true;

  std::vector<Figure *> bcp = bSpline->GetControlPoints();
  if (ImGui::BeginListBox("Bezier points")) {
    for (int i = 0; i < bcp.size(); i++) {
      if (ImGui::Selectable((bcp[i]->name + ' ').c_str(), &bcp[i]->selected)) {
        for (int j = 0; j < bcp.size(); j++) {
          if (j != i) {
            bcp[j]->selected = false;
          }
        }
      }
    }
    ImGui::EndListBox();
  }

  int idx = -1;
  for (int i = 0; i < bcp.size(); i++) {
    if (bcp[i]->selected) {
      idx = i;
      break;
    }
  }

  glm::vec3 diff = idx != -1 ? bcp[idx]->GetPosition() : glm::vec3(0.f);

  bool change = false;
  if (idx != -1 && bcp[idx]->CreatePositionImgui()) {
    change = true;
    diff = bcp[idx]->GetPosition() - diff;
  }

  if (change) {
    int mod = idx % 3;
    int bIdx = glm::floor(idx / 3.f);
    switch (mod) {
    case 0:
      //e_bIdx
      controlPoints[bIdx + 1]->SetPosition(
          controlPoints[bIdx + 1]->GetPosition() + diff);
      break;
    case 1:
      //f_bIdx
      controlPoints[bIdx + 1]->SetPosition(
          controlPoints[bIdx + 1]->GetPosition() + 2.f / 3 * diff);
      controlPoints[bIdx + 2]->SetPosition(
          controlPoints[bIdx + 2]->GetPosition() + 1.f / 3 * diff);
      break;
    case 2:
      //g_{bIdx+1}
      controlPoints[bIdx + 1]->SetPosition(
          controlPoints[bIdx + 1]->GetPosition() + 1.f / 3 * diff);
      controlPoints[bIdx + 2]->SetPosition(
          controlPoints[bIdx + 2]->GetPosition() + 2.f / 3 * diff);
      break;
    }
    RefreshBuffers();
    bcp = bSpline->GetControlPoints();
    bcp[idx]->selected = true;
  }
}
