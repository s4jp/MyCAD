#pragma once
#include "bezierC0.h"

class BezierInt : public BezierC0 {
private:
	std::tuple<std::vector<GLfloat>, std::vector<GLuint>>
	Calculate() const override;
	void virtual CalculateBspline() const;

protected:
    BezierC0 *bSpline;

public:
	BezierInt(int cpCountLoc, int segmentCountLoc, int segmentIdxLoc, int divisionLoc,
              const char *name = "Bezier Int");

	void Render(int colorLoc, int modelLoc) override;
};