#include "PivotableFigure.h"

PivotableFigure::PivotableFigure(glm::vec3 position, GLint modelLoc, std::string name, GLint colorLoc, bool numerate)
	: cScale(glm::vec3(0.f)), cAngle(glm::vec3(0.f)), cTranslation(glm::vec3(0.f)),
	SimpleFigure(position, modelLoc, name, colorLoc, numerate) {}

void PivotableFigure::CalculatePivotTransformation(glm::vec3 centerPosition, glm::vec3 centerScale, glm::vec3 centerAngle)
{
	glm::vec4 pos = glm::vec4(SimpleFigure::GetPosition(), 1.f);

	cTranslation = glm::vec3(
		CAD::translate(CAD::scaling(CAD::rotate(CAD::translate(glm::mat4(1.f),
			-centerPosition),
			centerAngle),
			centerScale),
			centerPosition) *
		pos -
		pos);

	cAngle = centerAngle;
	cScale = centerScale;

	CalculateModelMtx();
}

void PivotableFigure::SavePivotTransformations()
{
	SetPosition(GetPosition());
	cTranslation = glm::vec3(0.f);

	CalculateModelMtx();
}

glm::vec3 PivotableFigure::GetPosition()
{
	return SimpleFigure::GetPosition() + cTranslation;
}
