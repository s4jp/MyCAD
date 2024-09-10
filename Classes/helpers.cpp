#include "helpers.h"
#include <iostream>
#include <iomanip>

glm::mat4 CAD::translate(glm::mat4 matrix, glm::vec3 vector) {
  glm::mat4 translationMatrix = glm::mat4(1.0f);
  translationMatrix[3] = glm::vec4(vector, 1.0f);
  return translationMatrix * matrix;
}

glm::mat4 CAD::createXrotationMatrix(float angle) {
  glm::mat4 result = glm::mat4(1.0f);
  result[1][1] = cos(angle);
  result[1][2] = sin(angle);
  result[2][1] = -sin(angle);
  result[2][2] = cos(angle);
  return result;
}

glm::mat4 CAD::createYrotationMatrix(float angle) {
  glm::mat4 result = glm::mat4(1.0f);
  result[0][0] = cos(angle);
  result[0][2] = -sin(angle);
  result[2][0] = sin(angle);
  result[2][2] = cos(angle);
  return result;
}

glm::mat4 CAD::createZrotationMatrix(float angle) {
  glm::mat4 result = glm::mat4(1.0f);
  result[0][0] = cos(angle);
  result[0][1] = sin(angle);
  result[1][0] = -sin(angle);
  result[1][1] = cos(angle);
  return result;
}

glm::mat4 CAD::rotate(glm::mat4 matrix, glm::vec3 angle) {
  return CAD::createZrotationMatrix(angle.z) *
         CAD::createYrotationMatrix(angle.y) *
         CAD::createXrotationMatrix(angle.x) * matrix;
}

glm::mat4 CAD::projection(float fov, float ratio, float near,
                                 float far) {
  glm::mat4 result = glm::mat4(0.0f);
  result[0][0] = 1.0f / (tan(fov / 2.0f) * ratio);
  result[1][1] = 1.0f / tan(fov / 2.0f);
  result[2][2] = -(far + near) / (far - near);
  result[3][2] = (-2.0f * far * near) / (far - near);
  result[2][3] = -1.0f;
  result[3][3] = 0.0f;
  return result;
}

glm::mat4 CAD::scaling(glm::mat4 matrix, glm::vec3 scale) {
  glm::mat4 scaleMatrix = glm::mat4(1.0f);
  scaleMatrix[0][0] = scale.x;
  scaleMatrix[1][1] = scale.y;
  scaleMatrix[2][2] = scale.z;
  return scaleMatrix * matrix;
}

void CAD::printVector(glm::vec3 vec) {
  std::cout << "{ " << vec.x << " " << vec.y << " " << vec.z << " }"
            << std::endl;
}

std::vector<glm::vec3> CAD::circleIntersections(CAD::Sphere sphere,
                                                glm::vec3 rayCenter,
                                                glm::vec3 rayDir) {
  glm::vec3 centerDiff = rayCenter - sphere.center;
  std::vector<glm::vec3> result;

  float A = glm::dot(rayDir, rayDir);
  float B = 2 * glm::dot(centerDiff, rayDir);
  float C = glm::dot(centerDiff, centerDiff) - sphere.radius * sphere.radius;

  float delta = B * B - 4 * A * C;
  if (delta >= 0) {
    float k = (-B - glm::sqrt(delta)) / (2 * A);
    result.push_back(rayCenter + k * rayDir);
    if (delta > 0) {
      k = (-B + glm::sqrt(delta)) / (2 * A);
      result.push_back(rayCenter + k * rayDir);
    }
  }

  return result;
}

glm::mat4 CAD::lookAt(glm::vec3 position, glm::vec3 target, glm::vec3 up) {

  glm::vec3 f = glm::vec3(glm::normalize(target - position));
  glm::vec3 r = glm::vec3(glm::normalize(glm::cross(f, up)));
  glm::vec3 u = glm::vec3(glm::cross(r, f));
  glm::vec3 t = glm::vec3(-glm::dot(r, position), -glm::dot(u, position),
                          glm::dot(f, position));

  glm::mat4 result = glm::mat4(0.f);

  result[0][0] = r.x;
  result[1][0] = r.y;
  result[2][0] = r.z;

  result[0][1] = u.x;
  result[1][1] = u.y;
  result[2][1] = u.z;

  result[0][2] = -f.x;
  result[1][2] = -f.y;
  result[2][2] = -f.z;

  result[3] = glm::vec4(t, 1.f);
  return result;
}

void CAD::printMatrix(glm::mat4 mat) 
{
  for (int i = 0; i < 4; i++) {
    std::cout << "| ";
    for (int j = 0; j < 4; j++) {
      std::cout << std::left << std::setfill(' ') << std::setw(10) << mat[j][i]
                << " ";
    }
    std::cout << " |" << std::endl;
  }
}

float CAD::angleBetweenVectors(glm::vec3 u, glm::vec3 v) 
{ 
    return glm::acos(glm::clamp(glm::dot(u, v) / (glm::length(u) * glm::length(v)),-1.f,1.f));
}

std::string CAD::printPosition(glm::vec3 pos, std::string name) {
  return name + "X: " + std::to_string(pos.x) +
         ", Y:" + std::to_string(pos.y) + ", Z:" + std::to_string(pos.z);
}

std::tuple<glm::vec3, glm::vec3>
CAD::calculateNearFarProjections(double xMouse, double yMouse, glm::mat4 proj,
                            glm::mat4 view, Camera *camera) {
  glm::mat4 invMat = glm::inverse(proj * view);

  float xMouseClip =
      (xMouse - camera->GetWidth() / 2.0f) / (camera->GetWidth() / 2.0f);
  float yMouseClip =
      -1 * (yMouse - camera->GetHeight() / 2.0f) / (camera->GetHeight() / 2.0f);

  glm::vec4 near = glm::vec4(xMouseClip, yMouseClip, -1.0f, 1.0f);
  glm::vec4 far = glm::vec4(xMouseClip, yMouseClip, 1.0f, 1.0f);
  glm::vec4 nearResult = invMat * near;
  glm::vec4 farResult = invMat * far;
  nearResult /= nearResult.w;
  farResult /= farResult.w;
  return std::tuple<glm::vec3, glm::vec3>(glm::vec3(nearResult),
                                          glm::vec3(farResult));
}

glm::vec3 CAD::calculateNewCursorPos(GLFWwindow *window, glm::mat4 proj,
                                     glm::mat4 view, Camera *camera,
                                     int cursorRadius) {
  std::vector<glm::vec3> points = CAD::circleIntersections(
      CAD::Sphere(camera->Position, cursorRadius), camera->Position,
      CAD::calculateCameraRay(window, proj, view, camera));

  return points[1];
}

glm::vec3 CAD::calculateCameraRay(GLFWwindow *window, glm::mat4 proj,
                                  glm::mat4 view, Camera *camera) {
  double mouseX;
  double mouseY;
  glfwGetCursorPos(window, &mouseX, &mouseY);

  std::tuple<glm::vec3, glm::vec3> projections =
      CAD::calculateNearFarProjections(mouseX, mouseY, proj, view, camera);

  return glm::normalize(std::get<1>(projections) - std::get<0>(projections));
}

std::vector<glm::vec3> CAD::thomasAlgorihm(std::vector<glm::vec3> x,
                                           const std::vector<float> a,
                                           const std::vector<float> b,
                                           const std::vector<float> c) {
  // https://en.wikipedia.org/wiki/Tridiagonal_matrix_algorithm
  const int X = x.size();
  if (X < 1)
    return x;
  float *scratch = new float[X];

  scratch[0] = c[0] / b[0];
  x[0] = x[0] / b[0];

  for (int ix = 1; ix < X; ix++) {
    if (ix < X - 1) {
      scratch[ix] = c[ix] / (b[ix] - a[ix] * scratch[ix - 1]);
    }
    x[ix] = (x[ix] - a[ix] * x[ix - 1]) / (b[ix] - a[ix] * scratch[ix - 1]);
  }

  for (int ix = X - 2; ix >= 0; ix--)
    x[ix] -= scratch[ix] * x[ix + 1];

  return x;
}

glm::mat3x4 CAD::powerToBerensteinBasis(glm::mat3x4 in, float dist) { 
  glm::mat4 MB_E = glm::mat4(0.f);
  MB_E[0] = glm::vec4(+1.f, -3.f, +3.f, -1.f);
  MB_E[1] = glm::vec4(+0.f, +3.f, -6.f, +3.f);
  MB_E[2] = glm::vec4(+0.f, +0.f, +3.f, -3.f);
  MB_E[3] = glm::vec4(+0.f, +0.f, +0.f, +1.f);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      MB_E[j][i] /= glm::pow(dist, i);
    }
  }

  return glm::inverse(MB_E) * in;
}

glm::mat4 CAD::frustum(float left, float right, float bottom, float top,
                       float near, float far) {
  glm::mat4 result = glm::mat4(0.0f);
  result[0][0] = 2 * near / (right - left);
  result[1][1] = 2 * near / (top - bottom);
  result[2][0] = (right + left) / (right - left);
  result[2][1] = (top + bottom) / (top - bottom);
  result[2][2] = -(far + near) / (far - near);
  result[2][3] = -1.0f;
  result[3][2] = (-2.0f * far * near) / (far - near);
  return result;
}

void CAD::displacemt(float eyeSeparation, glm::mat4 &left, glm::mat4 &right) 
{
  left =
      CAD::translate(glm::mat4(1.f), glm::vec3(eyeSeparation / 2.f, 0.f, 0.f));
  right =
      CAD::translate(glm::mat4(1.f), glm::vec3(-eyeSeparation / 2.f, 0.f, 0.f));
}

glm::vec3 CAD::vec3casting(MG1::Float3 vec) { return glm::vec3(vec.x, vec.y, vec.z); }

int CAD::getPointIdx(MG1::PointRef pointRef) { return pointRef.GetId() - 1; }