#pragma once
#include "point.h"
#include "figure.h"

#include<vector>

class BicubicPatch : public Figure 
{
private:
	std::vector<Figure*> controlPoints;
	int* division;

	int segmentCountLoc;
	int segmentIdxLoc;
	int divisionLoc;

	std::tuple<std::vector<GLfloat>, std::vector<GLuint>> InitializeAndCalculate
	(int segmentCountLoc, int segmentIdxLoc, int divisionLoc, std::vector<Figure*> controlPoints, int* division);
	std::tuple<std::vector<GLfloat>, std::vector<GLuint>> Calculate() const;

public:
	BicubicPatch(int segmentCountLoc, int segmentIdxLoc, int divisionLoc, std::vector<Figure*> controlPoints, int* division);

	void Render(int colorLoc, int modelLoc);
	bool CreateImgui() { return false; };
	bool GetBoundingSphere(CAD::Sphere& sphere) { return false; };

	std::vector<Figure*> GetControlPoints() { return controlPoints; };
	void RefreshBuffers();
	void RenderMesh(int colorLoc, int modelLoc);
};