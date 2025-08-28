#include "IntersectionHelpers.h"
#include <iostream>
using namespace IntersectionConfig;

float IntersectionHelpers::DistanceSquared(Figure* A, float u, float v,
    Figure* B, float s, float t) {
    glm::vec3 p1 = A->GetValue(u, v);
    glm::vec3 p2 = B->GetValue(s, t);
    return glm::dot(p1 - p2, p1 - p2);
}

IntersectionHelpers::StartPoint IntersectionHelpers::RefinePoint(
    Figure* A, Figure* B, glm::vec2 uv, glm::vec2 st)
{
    glm::vec2 uvCurr = uv;
    glm::vec2 stCurr = st;

    for (int iter = 0; iter < GRADIENT_DESCENT_ITERS; iter++) {
        glm::vec3 P = A->GetValue(uvCurr.x, uvCurr.y);
        glm::vec3 Q = B->GetValue(stCurr.x, stCurr.y);
        glm::vec3 diff = P - Q;

        glm::vec3 dAu = A->GetTangentU(uvCurr.x, uvCurr.y);
        glm::vec3 dAv = A->GetTangentV(uvCurr.x, uvCurr.y);

        glm::vec3 dBs = B->GetTangentU(stCurr.x, stCurr.y);
        glm::vec3 dBt = B->GetTangentV(stCurr.x, stCurr.y);

        float df_du = 2.0f * glm::dot(diff, dAu);
        float df_dv = 2.0f * glm::dot(diff, dAv);
        float df_ds = -2.0f * glm::dot(diff, dBs);
        float df_dt = -2.0f * glm::dot(diff, dBt);

        uvCurr.x -= LEARNING_RATE * df_du;
        uvCurr.y -= LEARNING_RATE * df_dv;
        stCurr.x -= LEARNING_RATE * df_ds;
        stCurr.y -= LEARNING_RATE * df_dt;

        uvCurr = glm::clamp(uvCurr, glm::vec2(0.0f), glm::vec2(1.0f));
        stCurr = glm::clamp(stCurr, glm::vec2(0.0f), glm::vec2(1.0f));

        float err = glm::dot(diff, diff);
        if (err < TOLERANCE) {
            break;
        }
    }

    glm::vec3 Pfinal = A->GetValue(uvCurr.x, uvCurr.y);
    glm::vec3 Qfinal = B->GetValue(stCurr.x, stCurr.y);
    glm::vec3 mid = 0.5f * (Pfinal + Qfinal);

    StartPoint result;
    result.position = mid;
    result.uv1 = uvCurr;
    result.uv2 = stCurr;
    result.distance = glm::dot(Pfinal - Qfinal, Pfinal - Qfinal);

    return result;
}

bool IntersectionHelpers::AreUVsTooClose(glm::vec2 uv1, glm::vec2 uv2)
{
    return glm::length(uv1 - uv2) < IntersectionConfig::TOO_CLOSE_THRESHOLD;
}

IntersectionHelpers::StartPoint 
IntersectionHelpers::FindStartPoint(Figure* A, Figure* B)
{
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);


    for (int i = 0; i < MONTE_CARLO_SAMPLES; i++) {
        glm::vec2 uv(dist(rng), dist(rng));
        glm::vec2 st(dist(rng), dist(rng));

        float d2 = DistanceSquared(A, uv.x, uv.y, B, st.x, st.y);

        if (d2 < MONTE_CARLO_THRESHOLD && !AreUVsTooClose(uv,st)) {
            auto refined = RefinePoint(A, B, uv, st);

            if (refined.distance < START_POINT_ACCEPTANCE_THRESHOLD && !AreUVsTooClose(refined.uv1, refined.uv2)) {
                return refined;
            }
        }
    }

    return StartPoint(false);
}
