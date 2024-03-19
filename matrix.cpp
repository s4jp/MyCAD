#include<glm/glm.hpp>

static glm::mat4 translate(glm::mat4 matrix, glm::vec3 vector) {
  glm::mat4 translationMatrix = glm::mat4(1.0f);
  translationMatrix[3] = glm::vec4(vector, 1.0f);
  return translationMatrix * matrix;
}

static glm::mat4 createXrotationMatrix(float angle) {
  glm::mat4 result = glm::mat4(1.0f);
  result[1][1] = cos(angle);
  result[1][2] = sin(angle);
  result[2][1] = -sin(angle);
  result[2][2] = cos(angle);
  return result;
}

static glm::mat4 createYrotationMatrix(float angle) {
  glm::mat4 result = glm::mat4(1.0f);
  result[0][0] = cos(angle);
  result[0][2] = -sin(angle);
  result[2][0] = sin(angle);
  result[2][2] = cos(angle);
  return result;
}

static glm::mat4 createZrotationMatrix(float angle) {
  glm::mat4 result = glm::mat4(1.0f);
  result[0][0] = cos(angle);
  result[0][1] = sin(angle);
  result[1][0] = -sin(angle);
  result[1][1] = cos(angle);
  return result;
}

static glm::mat4 rotate(glm::mat4 matrix, glm::vec3 angle) {
  return createZrotationMatrix(angle[2]) * createYrotationMatrix(angle[1]) *
         createXrotationMatrix(angle[0]) * matrix;
}

static glm::mat4 projection(float fov, float ratio, float near, float far) {
  glm::mat4 result = glm::mat4(0.0f);
  result[0][0] = 1.0f / (tan(fov / 2.0f) * ratio);
  result[1][1] = 1.0f / tan(fov / 2.0f);
  result[2][2] = -(far + near) / (far - near);
  result[3][2] = (-2.0 * far * near) / (far - near);
  result[2][3] = -1.0f;
  result[3][3] = 0.0f;
  return result;
}

static glm::mat4 scaling(glm::mat4 matrix, glm::vec3 scale) {
  glm::mat4 scaleMatrix = glm::mat4(1.0f);
  scaleMatrix[0][0] = scale[0];
  scaleMatrix[1][1] = scale[1];
  scaleMatrix[2][2] = scale[2];
  return scaleMatrix * matrix;
}