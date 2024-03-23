#include "helpers.h"
#include <iostream>

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
