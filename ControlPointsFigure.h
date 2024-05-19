#pragma once
#include "ComplexFigure.h"
#include "PivotableFigure.h"

class ControlPointsFigure : public ComplexFigure {
protected:
	std::vector<PivotableFigure*> controlPoints;
public:
	ControlPointsFigure(GLint tessColorLoc, GLint modelLoc, std::string name, GLint colorLoc, bool numerate);

	std::vector<PivotableFigure*> GetControlPoints();
	void AddControlPoint(PivotableFigure* cp);
	bool RemoveControlPoint(int idx);
	void ClearControlPoints();

	virtual bool CreateImgui() override;
};