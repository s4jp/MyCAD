#include "BezierC2_2.h"
#include "Point2.h"
#include "PivotableFigure.h"

#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"

void BezierC2_2::CalculateBezier()
{
    // actual conversion
    int k = controlPoints.size();

    std::vector<glm::vec3> g;
    std::vector<glm::vec3> f;
    std::vector<glm::vec3> e;

    for (int i = 0; i < k - 2; i++) {
        g.push_back(1.f / 3 * controlPoints[i]->GetPosition() +
            2.f / 3 * controlPoints[i + 1]->GetPosition());
        f.push_back(2.f / 3 * controlPoints[i + 1]->GetPosition() +
            1.f / 3 * controlPoints[i + 2]->GetPosition());
        e.push_back(0.5f * g[i] + 0.5f * f[i]);
    }

    std::vector<glm::vec3> nCPs;

    for (int i = 0; i < k - 3; i++) {
        nCPs.push_back(e[i]);
        nCPs.push_back(f[i]);
        nCPs.push_back(g[i + 1]);
    }
    if (k - 3 > 0) {
        nCPs.push_back(e[k - 3]);
    }

    // sending data to bSpline
    bezier->ClearControlPoints();
    for (int i = 0; i < nCPs.size(); i++) {
        Point2* cp = new Point2(0.02F, nCPs[i], modelLoc, colorLoc, false);
        int mod = i % 3;
        int idx = glm::floor(i / 3.f);
        switch (mod) {
        case 0:
            cp->name = "e" + std::to_string(idx);
            break;
        case 1:
            cp->name = "f" + std::to_string(idx);
            break;
        case 2:
            cp->name = "g" + std::to_string(idx + 1);
            break;
        }
        bezier->AddControlPoint(cp);
    }
}

BezierC2_2::BezierC2_2(GLint cpCountLoc, GLint segmentCountLoc, GLint segmentIdxLoc, GLint tessColorLoc, GLint modelLoc, GLint colorLoc, bool numerate)
	: berensteinPolyline(false), ComplexCpFigure(cpCountLoc, segmentCountLoc, segmentIdxLoc, tessColorLoc, modelLoc, BezierC2_2::GetClassName(), colorLoc, numerate) {
	Calculate(false);
}

void BezierC2_2::Render()
{
    if (!berensteinPolyline) {
        vao.Bind();
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glLineWidth(1.0f);

        glUniform4fv(colorLoc, 1, glm::value_ptr(glm::vec4(0, 1, 0, 1)));
        glDrawElements(GL_LINE_STRIP, indices_count, GL_UNSIGNED_INT, 0);

        vao.Unbind();
        return;
    }

    bezier->Render();
    std::vector<PivotableFigure*> cp = bezier->GetControlPoints();
    for (int i = 0; i < cp.size(); i++) {
        cp[i]->Render();
    }
}

void BezierC2_2::RenderTess()
{
    bool temp = bezier->selected;
    bezier->selected = this->selected;
    bezier->RenderTess();
    bezier->selected = temp;
}

void BezierC2_2::Calculate(bool recalculate)
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

const std::string BezierC2_2::GetClassName()
{
	return "BezierC2";
}

void BezierC2_2::CreateBsplineImgui()
{
    this->berensteinPolyline = true;

    std::vector<PivotableFigure*> bcp = bezier->GetControlPoints();
    if (ImGui::BeginListBox("Bezier points")) {
        for (int i = 0; i < bcp.size(); i++) {
            if (ImGui::Selectable((bcp[i]->name + ' ').c_str(), &bcp[i]->selected)) {
                for (int j = 0; j < bcp.size(); j++) {
                    if (j != i) {
                        bcp[j]->selected = false;
                    }
                }
            }
        }
        ImGui::EndListBox();
    }

    int idx = -1;
    for (int i = 0; i < bcp.size(); i++) {
        if (bcp[i]->selected) {
            idx = i;
            break;
        }
    }

    glm::vec3 diff = idx != -1 ? bcp[idx]->GetPosition() : glm::vec3(0.f);

    bool change = false;
    if (idx != -1 && bcp[idx]->CreateImgui()) {
        change = true;
        diff = bcp[idx]->GetPosition() - diff;
    }

    if (change) {
        int mod = idx % 3;
        int bIdx = glm::floor(idx / 3.f);
        switch (mod) {
        case 0:
            //e_bIdx
            controlPoints[bIdx + 1]->SetPosition(
                controlPoints[bIdx + 1]->GetPosition() + diff);
            break;
        case 1:
            //f_bIdx
            controlPoints[bIdx + 1]->SetPosition(
                controlPoints[bIdx + 1]->GetPosition() + 2.f / 3 * diff);
            controlPoints[bIdx + 2]->SetPosition(
                controlPoints[bIdx + 2]->GetPosition() + 1.f / 3 * diff);
            break;
        case 2:
            //g_{bIdx+1}
            controlPoints[bIdx + 1]->SetPosition(
                controlPoints[bIdx + 1]->GetPosition() + 1.f / 3 * diff);
            controlPoints[bIdx + 2]->SetPosition(
                controlPoints[bIdx + 2]->GetPosition() + 2.f / 3 * diff);
            break;
        }
        Calculate(true);
        bcp = bezier->GetControlPoints();
        bcp[idx]->selected = true;
    }
}
