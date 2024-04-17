#pragma once
#include "bezierC0.h"

class BezierC2 : public BezierC0 {
private:
	 BezierC0* bSpline;
	 bool berensteinPolyline = false;

	 std::tuple<std::vector<GLfloat>, std::vector<GLuint>> Calculate() const override;
     void CalculateBspline() const;

public:

	BezierC2(int cpCountLoc, int segmentCountLoc, int segmentIdxLoc);

	void Render(int colorLoc, int modelLoc) override;
    void RenderPolyline(int colorLoc, int modelLoc) override;
    bool CreateImgui() override;
};