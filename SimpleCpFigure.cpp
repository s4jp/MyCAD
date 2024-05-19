#include "SimpleCpFigure.h"

SimpleCpFigure::SimpleCpFigure(GLint segmentCountLoc, GLint segmentIdxLoc, GLint tessColorLoc, GLint modelLoc, std::string name, GLint colorLoc, bool numerate)
	: segmentCountLoc(segmentCountLoc), segmentIdxLoc(segmentIdxLoc), ControlPointsFigure(tessColorLoc, modelLoc, name, colorLoc, numerate) {}
