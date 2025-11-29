#include "bicubicPatch.h"

#include <glm/gtc/type_ptr.hpp>

const int renderSegments = 10;

void BicubicPatch::RefreshBuffers()
{
    std::tuple<std::vector<GLfloat>, std::vector<GLfloat>, std::vector<GLuint>> data = Calculate();
    indices_count = std::get<2>(data).size();
    vbo.ReplaceBufferData(std::get<0>(data).data(),
        std::get<0>(data).size() * sizeof(GLfloat));
    ebo.ReplaceBufferData(std::get<2>(data).data(),
        std::get<2>(data).size() * sizeof(GLuint));

  for (size_t i = 0; i < std::get<0>(data).size() / 3; i++) {
    std::get<1>(data).push_back(0.f);
    std::get<1>(data).push_back(0.f);
  }
  uvVbo.ReplaceBufferData(std::get<1>(data).data(),
	  std::get<1>(data).size() * sizeof(GLfloat));
}

bool BicubicPatch::ReplaceControlPoint(int idx, Figure *cp) { 
  if (idx >= controlPoints.size())
    return false;

  controlPoints[idx] = cp;
  RefreshBuffers();
  return true;
}

void BicubicPatch::Render(int colorLoc, int modelLoc, bool grayscale)
{
    vao.Bind();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glLineWidth(1.0f);

    glUniform4fv(colorLoc, 1, glm::value_ptr(GetPolylineColor(grayscale)));

    for (int i = 0; i < 8; i++) {
        glDrawElements(GL_LINE_STRIP, 4, GL_UNSIGNED_INT, (void*)(4 * i * sizeof(GLuint)));
    }

    vao.Unbind();
}

std::tuple<std::vector<GLfloat>, std::vector<GLfloat>, std::vector<GLuint>> BicubicPatch::Calculate() const
{
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    for (int i = 0; i < controlPoints.size(); i++) {
        glm::vec3 pos = controlPoints[i]->GetPosition();
        vertices.push_back(pos.x);
        vertices.push_back(pos.y);
        vertices.push_back(pos.z);

        indices.push_back(i);
    }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            indices.push_back(i + 4 * j);
        }
    }

    return std::make_tuple(vertices, std::vector<GLfloat>(), indices);
}

std::tuple<std::vector<GLfloat>, std::vector<GLfloat>, std::vector<GLuint>> BicubicPatch::InitializeAndCalculate
(int cpCountLoc, int segmentCountLoc, int segmentIdxLoc, int divisionLoc, int otherAxisLoc, int bsplineLoc, bool bspline, std::vector<Figure*> controlPoints, int* division, int gregoryLoc, glm::vec2 uvOffset, int uvOffsetLoc)
{
	this->cpCountLoc = cpCountLoc;
    this->segmentCountLoc = segmentCountLoc;
    this->segmentIdxLoc = segmentIdxLoc;
    this->divisionLoc = divisionLoc;
    this->controlPoints = controlPoints;
    this->division = division;
	this->otherAxisLoc = otherAxisLoc;
    this->bspline = bspline;
	this->bsplineLoc = bsplineLoc;
	this->gregoryLoc = gregoryLoc;
	this->uvOffset = uvOffset;
	this->uvOffsetLoc = uvOffsetLoc;

    return Calculate();
}

BicubicPatch::BicubicPatch(int cpCountLoc, int segmentCountLoc, int segmentIdxLoc, int divisionLoc, int otherAxisLoc, int bsplineLoc, bool bspline, std::vector<Figure*> controlPoints, int* division, int gregoryLoc, glm::vec2 uvOffset, int uvOffsetLoc) :
    Figure(InitializeAndCalculate(cpCountLoc, segmentCountLoc, segmentIdxLoc, divisionLoc, otherAxisLoc, bsplineLoc, bspline, controlPoints, division, gregoryLoc, uvOffset, uvOffsetLoc), "Bicubic patch", glm::vec3(0.f)) 
{
    this->controlPoints = controlPoints;
}

void BicubicPatch::RenderTess(int colorLoc, int modelLoc, bool grayscale)
{
    vao.Bind();

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform4fv(colorLoc, 1, glm::value_ptr(GetColor(grayscale)));
    glUniform1i(segmentCountLoc, renderSegments);
    glLineWidth(3.0f);
    glUniform1i(divisionLoc, *division);

    glPatchParameteri(GL_PATCH_VERTICES, controlPoints.size());
    glUniform1i(cpCountLoc, controlPoints.size());

	glUniform1i(bsplineLoc, bspline);
	glUniform1i(gregoryLoc, false);

	glUniform2fv(uvOffsetLoc, 1, glm::value_ptr(uvOffset));

    for (int i = 0; i < renderSegments; i++) {
        glUniform1i(segmentIdxLoc, i);

        glUniform1i(otherAxisLoc, false);
        glDrawElements(GL_PATCHES, controlPoints.size(), GL_UNSIGNED_INT, 0);

        glUniform1i(otherAxisLoc, true);
        glDrawElements(GL_PATCHES, controlPoints.size(), GL_UNSIGNED_INT, 0);
    }

    vao.Unbind();
}

glm::vec3 BicubicPatch::GetValue(float u, float v, float R)
{
    u = glm::clamp(u, 0.0f, 1.0f);
    v = glm::clamp(v, 0.0f, 1.0f);

    glm::vec3 p(0.0f);

    if (!bspline) {
        auto B = [](float t, int i) {
            switch (i) {
            case 0: return (1 - t) * (1 - t) * (1 - t);
            case 1: return 3 * t * (1 - t) * (1 - t);
            case 2: return 3 * t * t * (1 - t);
            case 3: return t * t * t;
            }
            return 0.0f;
            };

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                glm::vec3 cp = controlPoints[i * 4 + j]->GetPosition();
                p += cp * B(u, i) * B(v, j);
            }
        }
    }
    else {
        auto N = [](float t, int i) {
            switch (i) {
            case 0: return (1 - t) * (1 - t) * (1 - t) / 6.0f;
            case 1: return (3 * t * t * t - 6 * t * t + 4) / 6.0f;
            case 2: return (-3 * t * t * t + 3 * t * t + 3 * t + 1) / 6.0f;
            case 3: return (t * t * t) / 6.0f;
            }
            return 0.0f;
            };

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                glm::vec3 cp = controlPoints[i * 4 + j]->GetPosition();
                p += cp * N(u, i) * N(v, j);
            }
        }
    }

    return OffsetAlongNormal(p, u, v, R);
}

glm::vec3 BicubicPatch::GetTangentU(float u, float v)
{
    return (GetValue(u + tangentEpsilon, v, 0) - GetValue(u - tangentEpsilon, v, 0))
        / (2.0f * tangentEpsilon);
}

glm::vec3 BicubicPatch::GetTangentV(float u, float v)
{
    return (GetValue(u, v + tangentEpsilon, 0) - GetValue(u, v - tangentEpsilon, 0))
        / (2.0f * tangentEpsilon);
}
