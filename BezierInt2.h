#pragma once
#include "ComplexCpFigure.h"

class BezierInt2 : public ComplexCpFigure {
private:
	void CalculateBezier() override;
public:
	BezierInt2(GLint cpCountLoc, GLint segmentCountLoc, GLint segmentIdxLoc, GLint tessColorLoc, GLint modelLoc, GLint colorLoc, bool numerate);

	void Render() override;
	void RenderTess() override;
	void Calculate(bool recalculate) override;
	const std::string GetClassName() override;
};