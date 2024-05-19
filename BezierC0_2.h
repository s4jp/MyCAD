#pragma once
#include "SimpleCpFigure.h"

class BezierC0_2 : public SimpleCpFigure {
private:
	GLint cpCountLoc;
public:
	BezierC0_2(GLint cpCountLoc, GLint segmentCountLoc, GLint segmentIdxLoc, GLint tessColorLoc, GLint modelLoc, GLint colorLoc, bool numerate);

	void Render() override;
	void RenderTess() override;
	void Calculate(bool recalculate) override;
	const std::string GetClassName() override;
};