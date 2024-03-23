#pragma once

#include<glm/glm.hpp>
#include<vector>

namespace CAD 
{
    class Sphere {
    public:
        glm::vec3 center;
        float radius;

        Sphere(){};

        Sphere(glm::vec3 center, float radius) {
          this->center = center;
          this->radius = radius;
        }
    };

	glm::mat4 translate(glm::mat4 matrix, glm::vec3 vector);
	glm::mat4 createXrotationMatrix(float angle);
    glm::mat4 createYrotationMatrix(float angle);
    glm::mat4 createZrotationMatrix(float angle);
    glm::mat4 rotate(glm::mat4 matrix, glm::vec3 angle);
    glm::mat4 projection(float fov, float ratio, float near, float far);
    glm::mat4 scaling(glm::mat4 matrix, glm::vec3 scale);
    void printVector(glm::vec3 vec);
    std::vector<glm::vec3> circleIntersections(CAD::Sphere sphere,
                                               glm::vec3 rayCenter,
                                               glm::vec3 rayDir);
}