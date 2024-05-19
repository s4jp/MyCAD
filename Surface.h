#pragma once
#include "ComplexFigure.h"
#include "BicubicPatch2.h"
#include "PivotableFigure.h"

class Surface : public ComplexFigure {
protected:
	std::vector<BicubicPatch2*> patches;
	int division;
	bool showMesh;
public:
	Surface(GLint tessColorLoc, GLint modelLoc, std::string name, GLint colorLoc, bool numerate);

	std::vector<PivotableFigure*> GetControlPoints();

	void Render() override;
	void RenderTess() override;
	bool CreateImgui() override;
	void Calculate(bool recalculate) override;

	virtual std::vector<PivotableFigure*> GeneratePlane(glm::vec3 center, int x, int y, float length, float width, GLint divisionLoc, GLint segmentCountLoc, GLint segmentIdxLoc) = 0;
	virtual std::vector<PivotableFigure*> GenerateCyliner(glm::vec3 center, int x, int y, float radius, float height, GLint divisionLoc, GLint segmentCountLoc, GLint segmentIdxLoc) = 0;
};