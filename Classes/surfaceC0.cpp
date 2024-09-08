#include "SurfaceC0.h"
#include "point.h"

std::vector<Figure*> SurfaceC0::CalculatePlane(int cpCount, int segmentCountLoc, int segmentIdxLoc, int divisionLoc, int otherAxisLoc, int bsplineLoc, int xSegments, int zSegments, float length, float width)
{
	float patchLength = length / xSegments;
	float patchLengthStep = patchLength / 3.f;
	float patchWidth = width / zSegments;
	float patchWidthStep = patchWidth / 3.f;

	glm::vec3 position = this->GetPosition();
	std::vector<Figure*> newPoints = std::vector<Figure*>();

	for (int i = 0; i < zSegments; i++) {
		for (int j = 0; j < xSegments; j++) {
			bool noBottom = (i == 0);
			bool noLeft = (j == 0);

			std::vector<Figure*> cps = std::vector<Figure*>();
			for (int k = 0; k < 16; k++) {
				if (!noBottom && glm::floor(k / 4) == 0) {
					cps.push_back((patches[(i - 1) * xSegments + j]->GetControlPoints())[k + 12]);
					continue;
				}
				if (!noLeft && k % 4 == 0) {
					cps.push_back((patches[i * xSegments + (j - 1)]->GetControlPoints())[k + 3]);
					continue;
				}

				float x = position.x + patchLength * (j - xSegments / 2.f) + (k % 4) * patchLengthStep;
				float z = position.z + patchWidth * (i - zSegments / 2.f) + glm::floor(k / 4) * patchWidthStep;
				Point* p = new Point(glm::vec3(x, position.y, z), 0.02F);
				cps.push_back(p);
				newPoints.push_back(p);
			}

			patches.push_back(new BicubicPatch(cpCount, segmentCountLoc, segmentIdxLoc, divisionLoc, otherAxisLoc, bsplineLoc, false, cps, &this->division));
		}
	}
	return newPoints;
}

std::vector<Figure*> SurfaceC0::CalculateCylinder(int cpCount, int segmentCountLoc, int segmentIdxLoc, int divisionLoc, int otherAxisLoc, int bsplineLoc, int xSegments, int zSegments, float radius, float height) {
	float patchRadius = 2 * M_PI / xSegments;
	float patchRadiusStep = patchRadius / 3.f;
	float patchHeight = height / zSegments;
	float patchHeightStep = patchHeight / 3.f;

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
			bool noBottom = (i == 0);
			bool noLeft = (j == 0);

			std::vector<Figure*> cps = std::vector<Figure*>();
			for (int k = 0; k < 16; k++) {
				if (!noBottom && glm::floor(k / 4) == 0) {
					cps.push_back((patches[(i - 1) * xSegments + j]->GetControlPoints())[k + 12]);
					continue;
				}
				if (!noLeft && k % 4 == 0) {
					cps.push_back((patches[i * xSegments + (j - 1)]->GetControlPoints())[k + 3]);
					continue;
				}
				if (j * 3 + k % 4 == xzPositions.size()) {
					if (j == 0)
						cps.push_back(cps[k - 3]);
					else
						cps.push_back((patches[i * xSegments]->GetControlPoints())[k - 3]);
					continue;
				}

				float x = xzPositions[j * 3 + k % 4].x;
				float y = position.y + patchHeight * (i - zSegments / 2.f) + glm::floor(k / 4) * patchHeightStep;
				float z = xzPositions[j * 3 + k % 4].y;
				Point* p = new Point(glm::vec3(x, y, z), 0.02F);
				cps.push_back(p);
				newPoints.push_back(p);
			}

			patches.push_back(new BicubicPatch(cpCount, segmentCountLoc, segmentIdxLoc, divisionLoc, otherAxisLoc, bsplineLoc, false, cps, &this->division));
		}
	}

	return newPoints;
}

SurfaceC0::SurfaceC0(glm::vec3 position, std::string name)
	: Figure(std::make_tuple(std::vector<GLfloat>(), std::vector<GLuint>()), name, position, true) {}

void SurfaceC0::RenderTess(int colorLoc, int modelLoc, bool grayscale)
{
	for (int i = 0; i < patches.size(); i++) {
		patches[i]->selected = selected;
		patches[i]->RenderTess(colorLoc, modelLoc, grayscale);
	}
}

bool SurfaceC0::CreateImgui()
{
	bool change = false;

	int prevValue = division;
	if (ImGui::InputInt("Division", &division)) {
		if (division < 2) {
			division = 2;
		}
		change = division != prevValue;
	}

	if (ImGui::Checkbox("Show mesh", &showMesh)) {
		change = true;
	}

	return change;
}

std::vector<Figure*> SurfaceC0::GetControlPoints()
{
	std::vector<Figure*> cps = std::vector<Figure*>();
	for (int i = 0; i < patches.size(); i++) {
		std::vector<Figure*> tempCps = patches[i]->GetControlPoints();
		for (int j = 0; j < tempCps.size(); j++) {
			cps.push_back(tempCps[j]);
		}
	}
	return cps;
}

void SurfaceC0::RefreshBuffers()
{
	for (int i = 0; i < patches.size(); i++) {
		patches[i]->RefreshBuffers();
	}
}

void SurfaceC0::Render(int colorLoc, int modelLoc, bool grayscale)
{
	if (!showMesh)
		return;

	for (int i = 0; i < patches.size(); i++) {
		patches[i]->Render(colorLoc, modelLoc, grayscale);
	}
}
