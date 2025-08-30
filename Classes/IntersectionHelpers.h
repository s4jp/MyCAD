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
	constexpr int NEWTON_MAX_ITERS = 1000;
    constexpr float EPS = 1e-3;
    constexpr int INNER_NEWTON_ITERS = 100;
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
                std::cout << "Start position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
                std::cout << "UV1: (" << uv1.x << ", " << uv1.y << ")" << std::endl;
                std::cout << "UV2: (" << uv2.x << ", " << uv2.y << ")" << std::endl;
                std::cout << "Distance: " << distance << std::endl << std::endl;
            } else {
                std::cout << "Invalid StartPoint" << std::endl << std::endl;
            }
		}
    };

    struct IntersectionPoint {
        glm::vec3 position;
        glm::vec2 uv1;
        glm::vec2 uv2;

		IntersectionPoint() = default;
        IntersectionPoint(glm::vec3 pos, glm::vec2 uv1,const glm::vec2 uv2)
            : position(pos), uv1(uv1), uv2(uv2) {}
        IntersectionPoint(StartPoint sp)
			: position(sp.position), uv1(sp.uv1), uv2(sp.uv2) {}
    };

    struct Intersection {
        bool isLoop;
        std::vector<IntersectionPoint> points;

        Intersection() : isLoop(false) {}
        Intersection(bool loop, std::vector<IntersectionPoint>&& pts)
            : isLoop(loop), points(std::move(pts)) {
        }

        void print() const {
            std::cout << "Intersection: " << (isLoop ? "Loop (closed)" : "Open") << "\n";

            std::cout << std::fixed << std::setprecision(6);

            for (size_t i = 0; i < points.size(); i++) {
                const auto& p = points[i];

                auto printFloat = [](float v, int w) {
                    std::cout << std::showpos
                        << std::setw(w) << std::setfill(' ') << v
                        << std::noshowpos;
                    };

                std::cout << "[" << i << "] Pos(";
                printFloat(p.position.x, 10); std::cout << ", ";
                printFloat(p.position.y, 10); std::cout << ", ";
                printFloat(p.position.z, 10); std::cout << ") uv1(";
                printFloat(p.uv1.x, 8); std::cout << ", ";
                printFloat(p.uv1.y, 8); std::cout << ") uv2(";
                printFloat(p.uv2.x, 8); std::cout << ", ";
                printFloat(p.uv2.y, 8); std::cout << ")\n";
            }

            std::cout << std::endl;

            std::cout.unsetf(std::ios::fixed | std::ios::showpos);
            std::cout.precision(6);
        }
    };

    static StartPoint FindStartPoint(Figure* A, Figure* B);
    static Intersection FindIntersection(Figure* A, Figure* B, StartPoint start, float step = 0.01f);

private:
    static float DistanceSquared(Figure* A, float u, float v, Figure* B, float s, float t);
    static StartPoint RefinePoint(Figure* A, Figure* B, glm::vec2 uv, glm::vec2 st);
	static bool AreUVsTooClose(glm::vec2 uv1, glm::vec2 uv2);
    static glm::vec4 Newton(Figure* A, Figure* B, glm::vec3 initialPoint, float step, glm::vec4 uv);
    static float WrapIfApplicable(float v, bool wrap);
	static glm::vec4 WrapIfApplicable(glm::vec4 v, Figure* A, Figure* B);
    static bool IsOutOfDomain(glm::vec4 v);
    static glm::vec3 GetPosition(Figure* A, Figure* B, glm::vec4 uv);
    static Intersection March(Figure* A, Figure* B, StartPoint start, float step);
    static glm::vec4 Clamp(glm::vec4 v);
};
