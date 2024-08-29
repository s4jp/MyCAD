#include "surfaceC2.h"
#include "point.h"

SurfaceC2::SurfaceC2(glm::vec3 position, std::string name) : SurfaceC0(position, name) {}

std::vector<Figure*> SurfaceC2::CalculatePlane(int cpCount, int segmentCountLoc, int segmentIdxLoc, int divisionLoc, int otherAxisLoc, int xSegments, int zSegments, float length, float width)
{
	int lengthSegmentCount = xSegments + 2;
	float segmentLength = length / lengthSegmentCount;
	int widthSegmentCount = zSegments + 2;
	float segmentWidth = width / widthSegmentCount;

	glm::vec3 position = this->GetPosition();
	std::vector<Figure*> newPoints = std::vector<Figure*>();

	for (int i = 0; i < zSegments; i++) {
		for (int j = 0; j < xSegments; j++) {
			bool firstInRow = (j == 0);
			bool firstInColumn = (i == 0);

			std::vector<Figure*> cps = std::vector<Figure*>();
			for (int k = 0; k < 16; k++) {
				if (!firstInRow && k % 4 != 3) {
					cps.push_back(patches[i * xSegments + (j - 1)]->GetControlPoints()[k + 1]);
					continue;
				}
				if (!firstInColumn && glm::floor(k / 4) != 3) {
					cps.push_back(patches[(i - 1) * xSegments + j]->GetControlPoints()[k + 4]);
					continue;
				}

				float x = position.x + (j + (k % 4) - lengthSegmentCount / 2.f) * segmentLength;
				float z = position.z + (i + glm::floor(k / 4) - widthSegmentCount / 2.f) * segmentWidth;
				Point* p = new Point(glm::vec3(x, position.y, z), 0.02F);
				cps.push_back(p);
				newPoints.push_back(p);
			}

			patches.push_back(new BicubicPatch(cpCount, segmentCountLoc, segmentIdxLoc, divisionLoc, otherAxisLoc, cps, &this->division));
		}
	}

	return newPoints;
}

std::vector<Figure*> SurfaceC2::CalculateCylinder(int cpCount, int segmentCountLoc, int segmentIdxLoc, int divisionLoc, int otherAxisLoc, int xSegments, int zSegments, float radius, float height)
{
	float patchRadius = 2 * M_PI / xSegments;
	float patchRadiusStep = patchRadius / 3.f;
	float patchHeight = height / zSegments;
	float patchHeightStep = patchHeight / 3.f;
	float patchHeightOverlap = 2.f * patchHeightStep;

	glm::vec3 position = this->GetPosition();
	std::vector<Figure*> newPoints = std::vector<Figure*>();

	std::vector<glm::vec2> xzPositions = std::vector<glm::vec2>();
	for (int i = 0; i < xSegments * 3; i++) {
		float x = position.x + radius * glm::cos(i * patchRadiusStep);
		float z = position.z + radius * glm::sin(i * patchRadiusStep);
		xzPositions.push_back(glm::vec2(x, z));
	}

	for (int i = 0; i < zSegments; i++) {
		for (int j = 0; j < xSegments; j++) {
			bool firstRow = (i == 0);				// generate 4 right cps
			bool firstColumn = (j == 0);			// generate 4 top cps

			std::vector<Figure*> cps = std::vector<Figure*>();
			for (int k = 0; k < 16; k++) {
				if (j == xSegments - 1 && k % 4 != 0) {
					cps.push_back(patches[i * xSegments]->GetControlPoints()[k - 1]);
					continue;
				}
				if (!firstColumn && firstRow && k % 4 != 3) {
					cps.push_back(patches[i * xSegments + (j - 1)]->GetControlPoints()[k + 1]);
					continue;
				}
				if (!firstRow && firstColumn && glm::floor(k / 4) != 3) {
					cps.push_back(patches[(i - 1) * xSegments + j]->GetControlPoints()[k + 4]);
					continue;
				}

				// TODO: fix center generation
				float x = xzPositions[j * 3 + k % 4].x;
				float y = position.y + patchHeight * (i - zSegments / 2.f) + glm::floor(k / 4) * patchHeightStep - i * patchHeightOverlap;
				float z = xzPositions[j * 3 + k % 4].y;
				Point* p = new Point(glm::vec3(x, position.y, z), 0.02F);
				cps.push_back(p);
				newPoints.push_back(p);
			}

			patches.push_back(new BicubicPatch(cpCount, segmentCountLoc, segmentIdxLoc, divisionLoc, otherAxisLoc, cps, &this->division));
		}
	}

	return newPoints;
}
