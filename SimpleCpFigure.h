#pragma once
#include "ControlPointsFigure.h"

class SimpleCpFigure : public ControlPointsFigure {
protected:
	GLint segmentCountLoc;
	GLint segmentIdxLoc;

	static const int renderSegments = 10;
public:
	SimpleCpFigure(GLint segmentCountLoc, GLint segmentIdxLoc, GLint tessColorLoc, GLint modelLoc, std::string name, GLint colorLoc, bool numerate);
};