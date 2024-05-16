#include "bicubicPatch.h"

#include <glm/gtc/type_ptr.hpp>

const int renderSegments = 10;

void BicubicPatch::RefreshBuffers()
{
    std::tuple<std::vector<GLfloat>, std::vector<GLuint>> data = Calculate();
    indices_count = std::get<1>(data).size();
    vbo.ReplaceBufferData(std::get<0>(data).data(),
        std::get<0>(data).size() * sizeof(GLfloat));
    ebo.ReplaceBufferData(std::get<1>(data).data(),
        std::get<1>(data).size() * sizeof(GLuint));
}

void BicubicPatch::Render(int colorLoc, int modelLoc)
{
    vao.Bind();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glLineWidth(1.0f);

    glUniform4fv(colorLoc, 1, glm::value_ptr(glm::vec4(0, 1, 0, 1)));

    for (int i = 0; i < 8; i++) {
        glDrawElements(GL_LINE_STRIP, 4, GL_UNSIGNED_INT, (void*)(4 * i * sizeof(GLuint)));
    }

    vao.Unbind();
}

std::tuple<std::vector<GLfloat>, std::vector<GLuint>> BicubicPatch::Calculate() const
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

    return std::make_tuple(vertices, indices);
}

std::tuple<std::vector<GLfloat>, std::vector<GLuint>> BicubicPatch::InitializeAndCalculate
(int segmentCountLoc, int segmentIdxLoc, int divisionLoc, std::vector<Figure*> controlPoints, int* division)
{
    this->segmentCountLoc = segmentCountLoc;
    this->segmentIdxLoc = segmentIdxLoc;
    this->divisionLoc = divisionLoc;
    this->controlPoints = controlPoints;
    this->division = division;

    return Calculate();
}

BicubicPatch::BicubicPatch(int segmentCountLoc, int segmentIdxLoc, int divisionLoc, std::vector<Figure*> controlPoints, int* division) :
    Figure(InitializeAndCalculate(segmentCountLoc, segmentIdxLoc, divisionLoc, controlPoints, division), "Bicubic patch", glm::vec3(0.f)) 
{
    this->controlPoints = controlPoints;
}

void BicubicPatch::RenderTess(int colorLoc, int modelLoc)
{
    vao.Bind();

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform4fv(colorLoc, 1, glm::value_ptr(GetColor()));
    glUniform1i(segmentCountLoc, renderSegments);
    glLineWidth(3.0f);
    glPatchParameteri(GL_PATCH_VERTICES, controlPoints.size());
    glUniform1i(divisionLoc, *division);

    for (int i = 0; i < renderSegments; i++) {
        glUniform1i(segmentIdxLoc, i);
        glDrawElements(GL_PATCHES, controlPoints.size(), GL_UNSIGNED_INT, 0);
    }

    vao.Unbind();
}

