#pragma once
#include "ControlPointsFigure.h"
#include "BezierC0_2.h"

class ComplexCpFigure : public ControlPointsFigure {
protected:
	BezierC0_2* bezier;

	virtual void CalculateBezier() = 0;
public:
	ComplexCpFigure(GLint cpCountLoc, GLint segmentCountLoc, GLint segmentIdxLoc, GLint tessColorLoc, GLint modelLoc, std::string name, GLint colorLoc, bool numerate);
};