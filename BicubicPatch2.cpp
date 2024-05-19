#include "BicubicPatch2.h"

#include <glm/gtc/type_ptr.hpp>

const std::string BicubicPatch2::GetClassName()
{
    return "Bicubic patch";
}

bool BicubicPatch2::CreateImgui()
{
    return false;
}

BicubicPatch2::BicubicPatch2(int* division, GLint divisionLoc, GLint segmentCountLoc, GLint segmentIdxLoc, GLint tessColorLoc, GLint modelLoc, GLint colorLoc, bool numerate)
    : division(division), divisionLoc(divisionLoc), SimpleCpFigure(segmentCountLoc, segmentIdxLoc, tessColorLoc, modelLoc, BicubicPatch2::GetClassName(), colorLoc, numerate) {
    Calculate(false);
}

void BicubicPatch2::Calculate(bool recalculate)
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

    if (recalculate) {
        RefreshBuffers(vertices, indices);
    }
    else {
        InitAndFillBuffers(vertices, indices);
    }
}

void BicubicPatch2::Render()
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

void BicubicPatch2::RenderTess()
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
