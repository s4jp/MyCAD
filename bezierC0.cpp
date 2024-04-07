#include "bezierC0.h"

#include <glm/gtc/type_ptr.hpp>

void BezierC0::RefreshBuffers() {
  std::tuple<std::vector<GLfloat>, std::vector<GLuint>> data = Calculate();
  indices_count = std::get<1>(data).size();
  vbo.ReplaceBufferData(std::get<0>(data).data(),
                        std::get<0>(data).size() * sizeof(GLfloat));
  ebo.ReplaceBufferData(std::get<1>(data).data(),
                        std::get<1>(data).size() * sizeof(GLuint));
}

std::tuple<std::vector<GLfloat>, std::vector<GLuint>>
BezierC0::Calculate() const {
  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;

  for (int i = 0; i < controlPoints.size(); i++) {
    glm::vec3 pos = controlPoints[i]->GetPosition();
    vertices.push_back(pos.x);
    vertices.push_back(pos.y);
    vertices.push_back(pos.z);

    indices.push_back(i);
  }

  return std::make_tuple(vertices, indices);
}

std::tuple<std::vector<GLfloat>, std::vector<GLuint>>
BezierC0::InitializeAndCalculate(std::vector<Point *> cps) {
  this->controlPoints = cps;

  return Calculate();
}

BezierC0::BezierC0(glm::vec3 position, std::vector<Point *> cps)
    : Figure(InitializeAndCalculate(cps), "Bezier C0", position, true) {}

void BezierC0::Render(int colorLoc, int modelLoc) {
  vao.Bind();
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glLineWidth(3.0f);

  glUniform4fv(colorLoc, 1, glm::value_ptr(GetColor()));
  glDrawElements(GL_LINE_STRIP, indices_count, GL_UNSIGNED_INT, 0);

  vao.Unbind();
}

bool BezierC0::CreateImgui() { return false; }

void BezierC0::AddControlPoint(Point *cp) {
  controlPoints.push_back(cp);
  RefreshBuffers();
}

bool BezierC0::RemoveControlPoint(int idx) {
  if (idx >= controlPoints.size())
    return false;
  controlPoints.erase(controlPoints.begin() + idx);
  RefreshBuffers();
}

