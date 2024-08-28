#pragma once
#include "figure.h"
#include "bicubicPatch.h"

#include<vector>

class PatchC0 : public Figure
{
private:
	std::vector<BicubicPatch*> patches = std::vector<BicubicPatch*>();
	int division = 4;
	bool showMesh = true;

public:
	PatchC0(glm::vec3 position);

	void Render(int colorLoc, int modelLoc);
	void RenderTess(int colorLoc, int modelLoc);
	bool CreateImgui();
	bool GetBoundingSphere(CAD::Sphere& sphere) { return false; };

	std::vector<Figure*> GetControlPoints();
	void RefreshBuffers();

	std::vector<Figure*> CalculatePlane(int cpCount, int segmentCountLoc, int segmentIdxLoc, int divisionLoc, int xSegments, int zSegments, float length, float width);
	//std::vector<Figure*> CalculateCylinder(int cpCount, int segmentCountLoc, int segmentIdxLoc, int divisionLoc, int xSegments, int zSegments, float radius, float height);
};