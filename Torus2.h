#pragma once

#include "PivotableFigure.h"

class Torus2 : public PivotableFigure {
private:
	float R1;
	float R2;
	int n1;
	int n2;

	glm::vec3 scale;
	glm::vec3 angle;
public:
	Torus2(float R1, float R2, int n1, int n2, glm::vec3 position, GLint modelLoc, GLint colorLoc, bool numerate);

	bool GetBoundingSphere(CAD::Sphere& sphere) override;
	void SavePivotTransformations() override;

	glm::vec3 GetScale();
	glm::vec3 GetAngle();
	void SetScale(glm::vec3 scale);
	void SetAngle(glm::vec3 angle);

	void Calculate(bool recalculate) override;
	void Render() override;
	std::string const GetClassName() override;
	bool CreateImgui() override;
	glm::mat4 CalculateModelMtx() override;
};