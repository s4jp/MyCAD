#include "SurfaceC0_2.h"
#include "Point2.h"
#include "BicubicPatch2.h"

SurfaceC0_2::SurfaceC0_2(GLint tessColorLoc, GLint modelLoc, GLint colorLoc, bool numerate)
	: Surface(tessColorLoc, modelLoc, SurfaceC0_2::GetClassName(), colorLoc, numerate) {}

const std::string SurfaceC0_2::GetClassName()
{
	return "Surface C0";
}

std::vector<PivotableFigure*> SurfaceC0_2::GeneratePlane(glm::vec3 center, int a, int b, float length, float width, GLint divisionLoc, GLint segmentCountLoc, GLint segmentIdxLoc)
{
	float patchLength = length / a;
	float patchLengthStep = patchLength / 3.f;
	float patchWidth = width / b;
	float patchWidthStep = patchWidth / 3.f;

	std::vector<PivotableFigure*> newPoints = std::vector<PivotableFigure*>();

	for (int i = 0; i < b; i++) {
		for (int j = 0; j < a; j++) {
			bool noBottom = (i == 0);
			bool noLeft = (j == 0);

			std::vector<PivotableFigure*> cps = std::vector<PivotableFigure*>();
			for (int k = 0; k < 16; k++) {
				if (!noBottom && glm::floor(k / 4) == 0) {
					cps.push_back((patches[(i - 1) * a + j]->GetControlPoints())[k + 12]);
					continue;
				}
				if (!noLeft && k % 4 == 0) {
					cps.push_back((patches[i * a + (j - 1)]->GetControlPoints())[k + 3]);
					continue;
				}

				float x = center.x + patchLength * (j - a / 2.f) + (k % 4) * patchLengthStep;
				float z = center.z + patchWidth * (i - b / 2.f) + glm::floor(k / 4) * patchWidthStep;
				Point2* p = new Point2(0.02F, glm::vec3(x, center.y, z), modelLoc, colorLoc, true);
				cps.push_back(p);
				newPoints.push_back(p);
			}

			patches.push_back(new BicubicPatch2(&this->division, divisionLoc, segmentCountLoc, segmentIdxLoc, tessColorLoc, modelLoc, colorLoc, false));
		}
	}
	Calculate(true);
	return newPoints;
}
