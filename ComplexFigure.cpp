#include "ComplexFigure.h"

#include "imgui.h"

ComplexFigure::ComplexFigure(GLint tessColorLoc, GLint modelLoc, std::string name, GLint colorLoc, bool numerate)
	: tessColorLoc(tessColorLoc), BaseFigure(modelLoc, name, colorLoc, numerate) {}

void ComplexFigure::CreateBsplineImgui()
{
	ImGui::Text("That ain't Bezier C2");
}
