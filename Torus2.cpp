#define _USE_MATH_DEFINES
#include "Torus2.h"

#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"

std::string const Torus2::GetClassName()
{
    return "Torus";
}

bool Torus2::CreateImgui()
{
    bool change = PivotableFigure::CreateImgui();

    ImGui::SeparatorText("Scale");
    if (ImGui::InputFloat("Sx", &scale.x, 0.01f, 1.f, "%.2f")) {
        CalculateModelMtx();
        change = true;
    }
    if (ImGui::InputFloat("Sy", &scale.y, 0.01f, 1.f, "%.2f")) {
        CalculateModelMtx();
        change = true;
    }
    if (ImGui::InputFloat("Sz", &scale.z, 0.01f, 1.f, "%.2f")) {
        CalculateModelMtx();
        change = true;
    }
    if (ImGui::Button("Reset scale")) {
        SetScale(glm::vec3(1.0f));
        change = true;
    }

    ImGui::SeparatorText("Rotation");
    if (ImGui::SliderAngle("X axis", &angle.x, -180.f, 180.f)) {
        CalculateModelMtx();
        change = true;
    }
    if (ImGui::SliderAngle("Y axis", &angle.y, -180.f, 180.f)) {
        CalculateModelMtx();
        change = true;
    }
    if (ImGui::SliderAngle("Z axis", &angle.z, -180.f, 180.f)) {
        CalculateModelMtx();
        change = true;
    }
    if (ImGui::Button("Reset rotation")) {
        SetAngle(glm::vec3(0.0f));
        change = true;
    }

    ImGui::SeparatorText("Options");
    if (ImGui::SliderFloat("R1", &R1, 0.01f, 5.f, "%.2f")) {
        Calculate(true);
        change = true;
    }
    if (ImGui::SliderFloat("R2", &R2, 0.01f, 5.f, "%.2f")) {
        Calculate(true);
        change = true;
    }
    if (ImGui::SliderInt("major", &n2, 3, 50)) {
        Calculate(true);
        change = true;
    }
    if (ImGui::SliderInt("minor", &n1, 3, 50)) {
        Calculate(true);
        change = true;
    }

    return change;
}

glm::mat4 Torus2::CalculateModelMtx()
{
    return PivotableFigure::CalculateModelMtx() 
        * CAD::rotate(glm::mat4(1.0f), GetAngle()) 
        * CAD::scaling(glm::mat4(1.0f), GetScale());
}

Torus2::Torus2(float R1, float R2, int n1, int n2, glm::vec3 position, GLint modelLoc, GLint colorLoc, bool numerate) 
    : R1(R1), R2(R2), n1(n1), n2(n2), scale(glm::vec3(0.f)), angle(glm::vec3(0.f)),
    PivotableFigure(position, modelLoc, GetClassName(), colorLoc, numerate) {
    Calculate(false);
}

bool Torus2::GetBoundingSphere(CAD::Sphere& sphere)
{
    return false;
}

void Torus2::SavePivotTransformations()
{
    SetAngle(angle + cAngle);
    cAngle = glm::vec3(0.f);

    SetScale(scale + cScale - glm::vec3(1.f));
    cScale = glm::vec3(1.f);

    PivotableFigure::SavePivotTransformations();
}

glm::vec3 Torus2::GetScale()
{
    return scale + cScale - glm::vec3(1.f);
}

glm::vec3 Torus2::GetAngle()
{
    return angle + cAngle;
}

void Torus2::SetScale(glm::vec3 scale)
{
    this->scale = scale;
    CalculateModelMtx();
}

void Torus2::SetAngle(glm::vec3 angle)
{
    this->angle = angle;

    while (angle.x > M_PI)
        angle.x -= M_PI * 2;
    while (angle.x < -M_PI)
        angle.x += M_PI * 2;

    while (angle.y > M_PI)
        angle.y -= M_PI * 2;
    while (angle.y < -M_PI)
        angle.y += M_PI * 2;

    while (angle.z > M_PI)
        angle.z -= M_PI * 2;
    while (angle.z < -M_PI)
        angle.z += M_PI * 2;

    CalculateModelMtx();
}

void Torus2::Calculate(bool recalculate)
{
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    float R1step = 2 * M_PI / n1;
    float R2step = 2 * M_PI / n2;

    for (int i = 0; i < n1; i++) {
        float xyElem = R1 + R2 * cos(i * R1step);
        float z = R2 * sin(i * R1step);

        for (int j = 0; j < n2; j++) {
            // some work-around
            glm::vec4 vertex = glm::vec4(
                xyElem * cos(j * R2step),                 // X
                xyElem * sin(j * R2step),                 // Y
                z,                                        // Z
                1.0f);
            vertex = CAD::rotate(glm::mat4(1.0f), glm::vec3(M_PI_2, 0.f, 0.f)) * vertex;
            vertices.push_back(vertex.x);
            vertices.push_back(vertex.y);
            vertices.push_back(vertex.z);

            // R2 loop
            indices.push_back(i * n2 + j);              // current
            indices.push_back(i * n2 + ((j + 1) % n2)); // next
            // R1 loop
            indices.push_back(i * n2 + j);              // current
            indices.push_back(((i + 1) % n1) * n2 + j); // next
        }
    }

    if (recalculate) {
        RefreshBuffers(vertices, indices);
        recalculate = false;
    }
    else {
        InitAndFillBuffers(vertices, indices);
    }
}

void Torus2::Render()
{
    vao.Bind();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glLineWidth(3.0f);

    glUniform4fv(colorLoc, 1, glm::value_ptr(GetColor()));
    glDrawElements(GL_LINES, indices_count, GL_UNSIGNED_INT, 0);

    vao.Unbind();
}
