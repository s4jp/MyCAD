#pragma once

#include<glm/glm.hpp>

namespace CAD 
{
	glm::mat4 translate(glm::mat4 matrix, glm::vec3 vector);
	glm::mat4 createXrotationMatrix(float angle);
    glm::mat4 createYrotationMatrix(float angle);
    glm::mat4 createZrotationMatrix(float angle);
    glm::mat4 rotate(glm::mat4 matrix, glm::vec3 angle);
    glm::mat4 projection(float fov, float ratio, float near, float far);
    glm::mat4 scaling(glm::mat4 matrix, glm::vec3 scale);
}