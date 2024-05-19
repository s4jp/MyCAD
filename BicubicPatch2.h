#pragma once
#include "SimpleCpFigure.h"

class BicubicPatch2 : public SimpleCpFigure {
private:
	int* division;
	GLint divisionLoc;
public:
	BicubicPatch2(int* division, GLint divisionLoc, GLint segmentCountLoc, GLint segmentIdxLoc, GLint tessColorLoc, GLint modelLoc, GLint colorLoc, bool numerate);

	void Render() override;
	void RenderTess() override;
	void Calculate(bool recalculate) override;
	const std::string GetClassName() override;
	bool CreateImgui() override;
};