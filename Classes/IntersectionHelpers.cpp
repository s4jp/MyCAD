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

IntersectionHelpers::Intersection IntersectionHelpers::FindIntersection(Figure* A, Figure* B, StartPoint start, float step)
{
	start.Print();
    A->Print();
	B->Print();
    std::cout << std::endl;

    std::vector<IntersectionPoint> curve;

    bool loop = false;

    auto forward = IntersectionHelpers::March(A, B, start, step);

    if (forward.isLoop) {
        return forward;
    }
    else {
		auto backward = IntersectionHelpers::March(A, B, start, -step);
        std::reverse(backward.points.begin(), backward.points.end());

        if (!backward.points.empty() && !forward.points.empty() &&
            glm::length(backward.points.back().position - forward.points.front().position) < IntersectionConfig::EPS)
        {
            backward.points.pop_back();
        }

        curve.insert(curve.end(), backward.points.begin(), backward.points.end());
        curve.insert(curve.end(), forward.points.begin(), forward.points.end());

        return Intersection(false, std::move(curve), backward.points.size());
    }
}

glm::vec4 IntersectionHelpers::Newton(Figure* A, Figure* B, glm::vec3 initialPoint, float step, glm::vec4 uv)
{
    glm::vec3 p = A->GetValue(uv.x, uv.y);
    glm::vec3 q = B->GetValue(uv.z, uv.w);

    glm::vec3 pdu = A->GetTangentU(uv.x, uv.y);
    glm::vec3 pdv = A->GetTangentV(uv.x, uv.y);
    glm::vec3 qdu = B->GetTangentU(uv.z, uv.w);
    glm::vec3 qdv = B->GetTangentV(uv.z, uv.w);

    glm::vec3 np = glm::normalize(glm::cross(pdu, pdv));
    glm::vec3 nq = glm::normalize(glm::cross(qdu, qdv));

    glm::vec3 t = glm::normalize(glm::cross(np, nq));

    glm::vec4 f(p - q, glm::dot(p - initialPoint, t) - step);

    glm::vec4 FdPu(pdu, glm::dot(pdu, t));
    glm::vec4 FdPv(pdv, glm::dot(pdv, t));
    glm::vec4 FdQu(-qdu, 0.0f);
    glm::vec4 FdQv(-qdv, 0.0f);

    glm::mat4 J(FdPu, FdPv, FdQu, FdQv);

    glm::vec4 delta = glm::inverse(J) * f;
    return delta;
}

float IntersectionHelpers::WrapIfApplicable(float v, bool wrap)
{
    if (wrap) {
        float wrapped = fmod(v, 1.0f);
        if (wrapped < 0.0f) wrapped += 1.0f;
        if (wrapped == 0.0f && v > 0.0f) wrapped = 1.0f;
        return wrapped;
    }
    else {
        return v;
    }
}

glm::vec4 IntersectionHelpers::WrapIfApplicable(glm::vec4 v, Figure* A, Figure* B)
{
    v.x = WrapIfApplicable(v.x, A->IsWrappedU());
    v.y = WrapIfApplicable(v.y, A->IsWrappedV());
    v.z = WrapIfApplicable(v.z, B->IsWrappedU());
    v.w = WrapIfApplicable(v.w, B->IsWrappedV());
	return v;
}

bool IntersectionHelpers::IsOutOfDomain(glm::vec4 v)
{
    return (v.x < 0.0f || v.x > 1.0f ||
        v.y < 0.0f || v.y > 1.0f ||
        v.z < 0.0f || v.z > 1.0f ||
        v.w < 0.0f || v.w > 1.0f);
}

glm::vec3 IntersectionHelpers::GetPosition(Figure* A, Figure* B, glm::vec4 uv)
{
    glm::vec3 pA = A->GetValue(uv.x, uv.y);
    glm::vec3 pB = B->GetValue(uv.z, uv.w);
    return 0.5f * (pA + pB);
}

IntersectionHelpers::Intersection IntersectionHelpers::March(Figure* A, Figure* B, StartPoint start, float step)
{
    std::vector<IntersectionPoint> pts;
    pts.push_back(IntersectionPoint(start));

    glm::vec4 last = { start.uv1, start.uv2 };

    for (int i = 0; i < IntersectionConfig::NEWTON_MAX_ITERS; i++) 
    {
        glm::vec4 next = last;
		glm::vec3 p0 = GetPosition(A, B, next);

        for (int j = 0; j < IntersectionConfig::INNER_NEWTON_ITERS; j++) 
        {
            glm::vec4 delta = Newton(A, B, p0, step, next);
            next -= delta;

            next = WrapIfApplicable(next, A, B);

            if (IsOutOfDomain(next)) {
				glm::vec4 clamped = Clamp(next);
                glm::vec3 posEdge = GetPosition(A, B, clamped);
                pts.emplace_back(posEdge, glm::vec2(clamped.x, clamped.y), glm::vec2(clamped.z, clamped.w));
                return Intersection(false, std::move(pts), 0);
            }

            float dist = glm::length(A->GetValue(next.x, next.y) - B->GetValue(next.z, next.w));
            //std::cout << "inner loop dist iter:" << j << " - " << dist << std::endl;

            if (dist < 0.02) break;
        }

        glm::vec3 pos = GetPosition(A, B, next);

        if (pts.size() > 6 && glm::length(pos - pts.front().position) < 0.7f * fabs(step)) {
            return Intersection(true, std::move(pts), 0);
        }

        pts.emplace_back(pos, glm::vec2(next.x, next.y), glm::vec2(next.z, next.w));
        last = next;
    }

    return Intersection(false, std::move(pts), 0);
}

glm::vec4 IntersectionHelpers::Clamp(glm::vec4 v)
{
    return glm::clamp(v, glm::vec4(0.0f), glm::vec4(1.0f));
}
