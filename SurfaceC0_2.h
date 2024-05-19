#pragma once
#include "Surface.h"

class SurfaceC0_2 : public Surface {
public:
	SurfaceC0_2(GLint tessColorLoc, GLint modelLoc, GLint colorLoc, bool numerate);

	const std::string GetClassName() override;
	std::vector<PivotableFigure*> GeneratePlane(glm::vec3 center, int x, int y, float length, float width, GLint divisionLoc, GLint segmentCountLoc, GLint segmentIdxLoc) override;
	//std::vector<PivotableFigure*> GenerateCyliner(glm::vec3 center, int x, int y, float radius, float height, GLint divisionLoc, GLint segmentCountLoc, GLint segmentIdxLoc) override;
};