#pragma once

#include "PivotableFigure.h"
#include "helpers.h"

class Point2 : public PivotableFigure {
private:
	float radius;
public:
	Point2(float radius, glm::vec3 position, GLint modelLoc, GLint colorLoc, bool numerate);

	bool GetBoundingSphere(CAD::Sphere& sphere) override;

	void Calculate(bool recalculate) override;
	void Render() override;
	std::string const GetClassName() override;
};