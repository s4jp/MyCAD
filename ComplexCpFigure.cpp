#include "ComplexCpFigure.h"

ComplexCpFigure::ComplexCpFigure(GLint cpCountLoc, GLint segmentCountLoc, GLint segmentIdxLoc, GLint tessColorLoc, GLint modelLoc, std::string name, GLint colorLoc, bool numerate)
	: bezier(new BezierC0_2(cpCountLoc, segmentCountLoc, segmentIdxLoc, tessColorLoc, modelLoc, colorLoc, false))
	, ControlPointsFigure(tessColorLoc, modelLoc, name, colorLoc, numerate) {}
