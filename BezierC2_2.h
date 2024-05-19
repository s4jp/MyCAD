#pragma once
#include "ComplexCpFigure.h"

class BezierC2_2 : public ComplexCpFigure {
private:
	bool berensteinPolyline;

	void CalculateBezier() override;
public:
	BezierC2_2(GLint cpCountLoc, GLint segmentCountLoc, GLint segmentIdxLoc, GLint tessColorLoc, GLint modelLoc, GLint colorLoc, bool numerate);

	void Render() override;
	void RenderTess() override;
	void Calculate(bool recalculate) override;
	const std::string GetClassName() override;
	void CreateBsplineImgui() override;
};