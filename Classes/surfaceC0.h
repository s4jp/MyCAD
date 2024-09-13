#pragma once
#include "figure.h"
#include "bicubicPatch.h"

#include <vector>
#include <string>

class SurfaceC0 : public Figure
{
protected:
	std::vector<BicubicPatch*> patches = std::vector<BicubicPatch*>();
	bool showMesh = false;

public:
    int division = 4;

	SurfaceC0(glm::vec3 position, std::string name = "Surface C0");

	void Render(int colorLoc, int modelLoc, bool grayscale);
    void RenderTess(int colorLoc, int modelLoc, bool grayscale);
	bool CreateImgui();
	bool GetBoundingSphere(CAD::Sphere& sphere) { return false; };

	std::vector<Figure*> GetControlPoints();
	void RefreshBuffers();

	std::vector<Figure*> virtual CalculatePlane(int cpCount, int segmentCountLoc, int segmentIdxLoc, int divisionLoc, int otherAxisLoc, int bsplineLoc, int xSegments, int zSegments, float length, float width);
	std::vector<Figure*> virtual CalculateCylinder(int cpCount, int segmentCountLoc, int segmentIdxLoc, int divisionLoc, int otherAxisLoc, int bsplineLoc, int xSegments, int zSegments, float radius, float height);

	void AddPatch(BicubicPatch *patch);
    int addToMG1Scene(MG1::Scene &scene, std::vector<uint32_t> cpsIdxs);
};