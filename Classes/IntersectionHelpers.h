#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <random>
#include <limits>
#include "Figure.h"
#include <iostream>

namespace IntersectionConfig {
    constexpr int MONTE_CARLO_SAMPLES = 50000;
    constexpr int GRADIENT_DESCENT_ITERS = 1000;
    constexpr float LEARNING_RATE = 0.01f;
    constexpr float TOLERANCE = 1e-5f;
    constexpr float MONTE_CARLO_THRESHOLD = 0.1f;
	constexpr float START_POINT_ACCEPTANCE_THRESHOLD = 1e-5f;
	constexpr float TOO_CLOSE_THRESHOLD = 1e-2f;
}

class IntersectionHelpers {
public:
    struct StartPoint {
        glm::vec3 position;
        glm::vec2 uv1; 
        glm::vec2 uv2;
        float distance;
		bool valid;

		StartPoint() : position(0.0f), uv1(0.0f), uv2(0.0f), distance(std::numeric_limits<float>::max()), valid(true) {}
		StartPoint(bool isValid) : position(0.0f), uv1(0.0f), uv2(0.0f), distance(std::numeric_limits<float>::max()), valid(isValid) {}
        void const Print() {
            if (valid) {
                std::cout << "Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
                std::cout << "UV1: (" << uv1.x << ", " << uv1.y << ")" << std::endl;
                std::cout << "UV2: (" << uv2.x << ", " << uv2.y << ")" << std::endl;
                std::cout << "Distance: " << distance << std::endl << std::endl;
            } else {
                std::cout << "Invalid StartPoint" << std::endl << std::endl;
            }
		}
    };

    static StartPoint FindStartPoint(Figure* A, Figure* B);

private:
    static float DistanceSquared(Figure* A, float u, float v, Figure* B, float s, float t);
    static StartPoint RefinePoint(Figure* A, Figure* B, glm::vec2 uv, glm::vec2 st);
	static bool AreUVsTooClose(glm::vec2 uv1, glm::vec2 uv2);
};
