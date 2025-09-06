#pragma once
#include "bezierC0.h"

class BezierInt : public BezierC0 {
private:
	std::tuple<std::vector<GLfloat>, std::vector<GLuint>>
	Calculate() const override;
	void virtual CalculateBspline() const;

public:
	BezierC0* bSpline;

	BezierInt(int cpCountLoc, int segmentCountLoc, int segmentIdxLoc, int divisionLoc,
              const char *name = "Bezier Int");

	void Render(int colorLoc, int modelLoc, bool grayscale) override;
    int Serialize(MG1::Scene &scene, std::vector<uint32_t> cpsIdxs) override;
};