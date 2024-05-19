#pragma once

#include "SimpleFigure.h"
#include "helpers.h"

class PivotableFigure : public SimpleFigure {
protected:
	glm::vec3 cScale;
	glm::vec3 cTranslation;
	glm::vec3 cAngle;
public:
	PivotableFigure(glm::vec3 position, GLint modelLoc, std::string name, GLint colorLoc, bool numerate);

	virtual bool GetBoundingSphere(CAD::Sphere& sphere) = 0;
	void CalculatePivotTransformation(glm::vec3 centerPosition, glm::vec3 centerScale, glm::vec3 centerAngle);
	virtual void SavePivotTransformations();
	glm::vec3 GetPosition() override;
};