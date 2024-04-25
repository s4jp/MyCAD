#pragma once
#include "bezierInt.h"

class BezierC2 : public BezierInt {
private:
	 bool berensteinPolyline = false;

     void CalculateBspline() const override;

public:
	BezierC2(int cpCountLoc, int segmentCountLoc, int segmentIdxLoc);

    void RenderPolyline(int colorLoc, int modelLoc) override;
    bool CreateImgui() override;
    void CreateBsplineImgui() override;
};