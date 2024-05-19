#include "BaseFigure.h"

#include "imgui.h"

int BaseFigure::counter = 0;

BaseFigure::BaseFigure(GLint modelLoc, std::string name, GLint colorLoc, bool numerate)
    : id(numerate ? counter++ : -1), name(name + " #" + std::to_string(id)), colorLoc(colorLoc), selected(false), indices_count(0), modelLoc(modelLoc), model(glm::mat4(1.f)) {}

BaseFigure::~BaseFigure()
{
    vao.Delete();
    vbo.Delete();
    ebo.Delete();
}

void BaseFigure::InitAndFillBuffers(std::vector<GLfloat> vertices, std::vector<GLuint> indices)
{
    vao.Bind();
    vbo = VBO(vertices.data(), vertices.size() * sizeof(GLfloat));
    ebo = EBO(indices.data(), indices.size() * sizeof(GLint));
    indices_count = indices.size();
    vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, 0, (void*)0);
    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();
}

void BaseFigure::RefreshBuffers(std::vector<GLfloat> vertices, std::vector<GLuint> indices)
{
    vbo.ReplaceBufferData(vertices.data(), vertices.size() * sizeof(GLfloat));
    ebo.ReplaceBufferData(indices.data(), indices.size() * sizeof(GLint));
    indices_count = indices.size();
}

glm::vec4 BaseFigure::GetColor() const
{
    return selected ? glm::vec4(0.89f, 0.29f, 0.15f, 1.0f)
        : glm::vec4(1.0f, 0.73f, 0.31f, 1.0f);
}

bool BaseFigure::CreateImgui()
{
    ImGui::SeparatorText((name + " options:").c_str());
    return false;
}
