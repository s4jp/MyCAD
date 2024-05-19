#pragma once
#include "BaseFigure.h"

class ComplexFigure : public BaseFigure {
protected:
	GLint tessColorLoc;
public:
	ComplexFigure(GLint tessColorLoc, GLint modelLoc, std::string name, GLint colorLoc, bool numerate);

	virtual void RenderTess() = 0;
	virtual void CreateBsplineImgui();
};