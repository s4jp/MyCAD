#include "Surface.h"

#include "imgui.h"

Surface::Surface(GLint tessColorLoc, GLint modelLoc, std::string name, GLint colorLoc, bool numerate)
	: patches(std::vector<BicubicPatch2*>()), division(4), showMesh(false), ComplexFigure(tessColorLoc, modelLoc, name, colorLoc, numerate) {}

std::vector<PivotableFigure*> Surface::GetControlPoints()
{
	std::vector<PivotableFigure*> cps = std::vector<PivotableFigure*>();
	for (int i = 0; i < patches.size(); i++) {
		std::vector<PivotableFigure*> tempCps = patches[i]->GetControlPoints();
		for (int j = 0; j < tempCps.size(); j++) {
			cps.push_back(tempCps[j]);
		}
	}
	return cps;
}

void Surface::Render()
{
	if (!showMesh)
		return;

	for (int i = 0; i < patches.size(); i++) {
		patches[i]->Render();
	}
}

void Surface::RenderTess()
{
	for (int i = 0; i < patches.size(); i++) {
		bool temp = patches[i]->selected;
		patches[i]->selected = selected;
		patches[i]->RenderTess();
		patches[i]->selected = temp;
	}
}

bool Surface::CreateImgui()
{
	bool change = false;

	int prevValue = division;
	if (ImGui::InputInt("Division", &division)) {
		if (division < 1) {
			division = 1;
		}
		change = division != prevValue;
	}

	if (ImGui::Checkbox("Bezier mesh", &showMesh)) {
		change = true;
	}

	return change;
}

void Surface::Calculate(bool recalculate)
{
	for (int i = 0; i < patches.size(); i++) {
		patches[i]->Calculate(true);
	}
}
