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

	bool bspline;
	int bsplineLoc;
	int gregoryLoc;

	int uvOffsetLoc;

	std::tuple<std::vector<GLfloat>, std::vector<GLfloat>, std::vector<GLuint>> InitializeAndCalculate
	(int cpCountLoc, int segmentCountLoc, int segmentIdxLoc, int divisionLoc, int otherAxisLoc, int bsplineLoc, bool bspline, std::vector<Figure*> controlPoints, int* division, int gregoryLoc, glm::vec2 uvOffset, int uvOffsetLoc);
	std::tuple<std::vector<GLfloat>, std::vector<GLfloat>, std::vector<GLuint>> Calculate() const;

public:
	glm::vec2 uvOffset;

	BicubicPatch(int cpCountLoc, int segmentCountLoc, int segmentIdxLoc, int divisionLoc, int otherAxisLoc, int bsplineLoc, bool bspline, std::vector<Figure*> controlPoints, int* division, int gregoryLoc, glm::vec2 uvOffset, int uvOffsetLoc);

	void Render(int colorLoc, int modelLoc, bool grayscale);
    void RenderTess(int colorLoc, int modelLoc, bool grayscale);
	bool CreateImgui() { return false; };
	bool GetBoundingSphere(CAD::Sphere& sphere) { return false; };

	std::vector<Figure*> GetControlPoints() { return controlPoints; };
	void RefreshBuffers();

	bool ReplaceControlPoint(int idx, Figure *cp);
	float GetR() { return 0; };

	bool Intersectional() { return false; }
	glm::vec3 GetValue(float u, float v) override;
	glm::vec3 GetTangentU(float u, float v) override;
	glm::vec3 GetTangentV(float u, float v) override;
};