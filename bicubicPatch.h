#pragma once
#include "point.h"
#include "figure.h"

#include<vector>

class BicubicPatch : public Figure 
{
private:
	std::vector<Figure*> controlPoints;
	int* division;

	int cpCountLoc;
	int segmentCountLoc;
	int segmentIdxLoc;
	int divisionLoc;
	int otherAxisLoc;

	std::tuple<std::vector<GLfloat>, std::vector<GLuint>> InitializeAndCalculate
	(int cpCountLoc, int segmentCountLoc, int segmentIdxLoc, int divisionLoc, int otherAxisLoc, std::vector<Figure*> controlPoints, int* division);
	std::tuple<std::vector<GLfloat>, std::vector<GLuint>> Calculate() const;

public:
	BicubicPatch(int cpCountLoc, int segmentCountLoc, int segmentIdxLoc, int divisionLoc, int otherAxisLoc, std::vector<Figure*> controlPoints, int* division);

	void Render(int colorLoc, int modelLoc);
	void RenderTess(int colorLoc, int modelLoc);
	bool CreateImgui() { return false; };
	bool GetBoundingSphere(CAD::Sphere& sphere) { return false; };

	std::vector<Figure*> GetControlPoints() { return controlPoints; };
	void RefreshBuffers();
};