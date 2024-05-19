#include "BezierC0_2.h"

#include <glm/gtc/type_ptr.hpp>

const std::string BezierC0_2::GetClassName()
{
    return "BezierC0";
}

BezierC0_2::BezierC0_2(GLint cpCountLoc, GLint segmentCountLoc, GLint segmentIdxLoc, GLint tessColorLoc, GLint modelLoc, GLint colorLoc, bool numerate)
    : cpCountLoc(cpCountLoc), SimpleCpFigure(segmentCountLoc, segmentIdxLoc, tessColorLoc, modelLoc, BezierC0_2::GetClassName(), colorLoc, numerate) {
    Calculate(false);
}

void BezierC0_2::Calculate(bool recalculate)
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

    if (recalculate) {
        RefreshBuffers(vertices, indices);
    }
    else {
        InitAndFillBuffers(vertices, indices);
    }
}

void BezierC0_2::Render()
{
    vao.Bind();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glLineWidth(1.0f);

    glUniform4fv(colorLoc, 1, glm::value_ptr(glm::vec4(0, 1, 0, 1)));
    glDrawElements(GL_LINE_STRIP, indices_count, GL_UNSIGNED_INT, 0);

    vao.Unbind();
}

void BezierC0_2::RenderTess()
{
    vao.Bind();

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform4fv(colorLoc, 1, glm::value_ptr(GetColor()));
    glUniform1i(segmentCountLoc, renderSegments);
    glLineWidth(3.0f);

    int count = glm::ceil((indices_count - 1) / 3.0f);
    for (int i = 0; i < count; i++) {
        int offset = 3 * i;
        int size = glm::min(4, (int)indices_count - offset);
        glUniform1i(cpCountLoc, size);
        glPatchParameteri(GL_PATCH_VERTICES, size);

        for (int j = 0; j < renderSegments; j++) {
            glUniform1i(segmentIdxLoc, j);
            glDrawElements(GL_PATCHES, 4, GL_UNSIGNED_INT,
                (void*)(offset * sizeof(GLuint)));
        }
    }

    vao.Unbind();
}
