#include "SimpleFigure.h"
#include "helpers.h"

#include "imgui.h"

SimpleFigure::SimpleFigure(glm::vec3 position, GLint modelLoc, std::string name, GLint colorLoc, bool numerate)
	: position(position), BaseFigure(modelLoc, name, colorLoc, numerate) {}

glm::mat4 SimpleFigure::CalculateModelMtx()
{
	return CAD::translate(glm::mat4(1.0f), GetPosition());
}

glm::vec3 SimpleFigure::GetPosition()
{
	return position;
}

void SimpleFigure::SetPosition(glm::vec3 position)
{
	this->position = position;
	model = CalculateModelMtx();
}

bool SimpleFigure::CreateImgui()
{
	bool change = BaseFigure::CreateImgui();
	ImGui::SeparatorText((name + " options:").c_str());

	ImGui::Text("Position: ");
	glm::vec3 pos = GetPosition();
	if (ImGui::InputFloat("X ", &pos.x, 0.01f, 1.f, "%.02f")) {
		SetPosition(pos);
		change = true;
	}
	if (ImGui::InputFloat("Y ", &pos.y, 0.01f, 1.f, "%.02f")) {
		SetPosition(pos);
		change = true;
	}
	if (ImGui::InputFloat("Z ", &pos.z, 0.01f, 1.f, "%.02f")) {
		SetPosition(pos);
		change = true;
	}

	return change;
}
