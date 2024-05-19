#pragma once

#include "SimpleFigure.h"

class Grid2 : public SimpleFigure {
private:
	float size;
	int division;
public:
	Grid2(float size, int division, glm::vec3 position, GLint modelLoc, GLint colorLoc, bool numerate);

	void Calculate(bool recalculate) override;
	void Render() override;
	std::string const GetClassName() override;
};