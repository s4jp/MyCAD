#pragma once

#include "SimpleFigure.h"

class Cursor2 : public SimpleFigure {
private:
	float length;
public:
	Cursor2(float length, glm::vec3 position, GLint modelLoc, GLint colorLoc, bool numerate);

	void Calculate(bool recalculate) override;
	void Render() override;
	std::string const GetClassName() override;
};