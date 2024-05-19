#include "BezierInt2.h"
#include "Point2.h"

#include <glm/gtc/type_ptr.hpp>

void BezierInt2::Calculate(bool recalculate)
{
    CalculateBezier();

    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    for (int i = 0; i < controlPoints.size(); i++) {
        glm::vec3 pos = controlPoints[i]->GetPosition();
        vertices.push_back(pos.x);
        vertices.push_back(pos.y);
        vertices.push_back(pos.z);

        indices.push_back(i);
    }

    if (recalculate) {
        RefreshBuffers(vertices, indices);
    }
    else {
        InitAndFillBuffers(vertices, indices);
    }
}

const std::string BezierInt2::GetClassName()
{
    return "BezierInt";
}

BezierInt2::BezierInt2(GLint cpCountLoc, GLint segmentCountLoc, GLint segmentIdxLoc, GLint tessColorLoc, GLint modelLoc, GLint colorLoc, bool numerate)
    : ComplexCpFigure(cpCountLoc, segmentCountLoc, segmentIdxLoc, tessColorLoc, modelLoc, BezierInt2::GetClassName(), colorLoc, numerate) {
    Calculate(false);
}

void BezierInt2::Render()
{
    vao.Bind();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glLineWidth(1.0f);

    glUniform4fv(colorLoc, 1, glm::value_ptr(glm::vec4(0, 1, 0, 1)));
    glDrawElements(GL_LINE_STRIP, indices_count, GL_UNSIGNED_INT, 0);

    vao.Unbind();
}

void BezierInt2::RenderTess()
{
    bool temp = bezier->selected;
    bezier->selected = this->selected;
    bezier->RenderTess();
    bezier->selected = temp;
}

void BezierInt2::CalculateBezier()
{
    std::vector<glm::vec3> a;
    for (int i = 0; i < controlPoints.size(); i++) {
        a.push_back(controlPoints[i]->GetPosition());
    }

    std::vector<float> dist;
    for (int i = 1; i < a.size(); i++) {
        float distance = glm::distance(a[i], a[i - 1]);
        if (distance < 1E-4) {
            a.erase(a.begin() + i);
            i--;
        }
        else
            dist.push_back(distance);
    }

    const int N = (int)a.size() - 1;
    std::vector<float> alpha, beta, diagonal;
    std::vector<glm::vec3> R;

    for (int i = 1; i < N; i++) {
        // alpha[0] and beta[N-1] won't be used
        alpha.push_back(dist[i - 1] / (dist[i - 1] + dist[i]));
        beta.push_back(dist[i] / (dist[i - 1] + dist[i]));
        R.push_back(3.f *
            ((a[i + 1] - a[i]) / dist[i] -
                (a[i] - a[i - 1]) / dist[i - 1]) /
            (dist[i - 1] + dist[i]));
        diagonal.push_back(2.f);
    }

    std::vector<glm::vec3> c;
    c.push_back(glm::vec3(0.f));
    std::vector<glm::vec3> thomas =
        CAD::thomasAlgorihm(R, alpha, diagonal, beta);
    c.insert(c.end(), thomas.cbegin(), thomas.cend());
    c.push_back(glm::vec3(0.f));

    std::vector<glm::vec3> d;
    for (int i = 1; i < N + 1; i++) {
        d.push_back((c[i] - c[i - 1]) / (3.f * dist[i - 1]));
    }

    std::vector<glm::vec3> b;
    for (int i = 1; i < N + 1; i++) {
        b.push_back((a[i] - a[i - 1] - c[i - 1] * dist[i - 1] * dist[i - 1] -
            d[i - 1] * dist[i - 1] * dist[i - 1] * dist[i - 1]) /
            dist[i - 1]);
    }

    bezier->ClearControlPoints();
    for (int i = 0; i < N; i++) {
        glm::mat3x4 temp = glm::mat3x4(0.f);
        temp[0] = glm::vec4(a[i].x, b[i].x, c[i].x, d[i].x);
        temp[1] = glm::vec4(a[i].y, b[i].y, c[i].y, d[i].y);
        temp[2] = glm::vec4(a[i].z, b[i].z, c[i].z, d[i].z);

        temp = CAD::powerToBerensteinBasis(temp, dist[i]);

        bezier->AddControlPoint(
            new Point2(0.05F, glm::vec3(temp[0][0], temp[1][0], temp[2][0]), modelLoc, colorLoc, false));
        bezier->AddControlPoint(
            new Point2(0.05F, glm::vec3(temp[0][1], temp[1][1], temp[2][1]), modelLoc, colorLoc, false));
        bezier->AddControlPoint(
            new Point2(0.05F, glm::vec3(temp[0][2], temp[1][2], temp[2][2]), modelLoc, colorLoc, false));
        if (i == N - 1)
            bezier->AddControlPoint(new Point2(
                0.05F, glm::vec3(temp[0][3], temp[1][3], temp[2][3]), modelLoc, colorLoc, false));
    }
}