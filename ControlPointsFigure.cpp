#include "ControlPointsFigure.h"

#include "imgui.h"

ControlPointsFigure::ControlPointsFigure(GLint tessColorLoc, GLint modelLoc, std::string name, GLint colorLoc, bool numerate)
	: controlPoints(std::vector<PivotableFigure*>()), ComplexFigure(tessColorLoc, modelLoc, name, colorLoc, numerate) {}

std::vector<PivotableFigure*> ControlPointsFigure::GetControlPoints()
{
	return controlPoints;
}

void ControlPointsFigure::AddControlPoint(PivotableFigure* cp)
{
	controlPoints.push_back(cp);
	Calculate(true);
}

bool ControlPointsFigure::RemoveControlPoint(int idx)
{
	if (idx >= controlPoints.size())
		return false;
	controlPoints.erase(controlPoints.begin() + idx);
	Calculate(true);
	return true;
}

void ControlPointsFigure::ClearControlPoints()
{
	controlPoints.clear();
	Calculate(true);
}

bool ControlPointsFigure::CreateImgui()
{
	bool change = ComplexFigure::CreateImgui();

	if (ImGui::BeginListBox("Control points")) {
		for (int i = 0; i < controlPoints.size(); i++) {
			if (ImGui::Selectable((controlPoints[i]->name + ' ').c_str())) {
				RemoveControlPoint(i);
				change = true;
			}
		}
		ImGui::EndListBox();
	}
	return change;
}
