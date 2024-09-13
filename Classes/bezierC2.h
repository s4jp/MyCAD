#pragma once
#include "bezierInt.h"

class BezierC2 : public BezierInt {
private:
	 bool berensteinPolyline = false;

     void CalculateBspline() const override;

public:
	BezierC2(int cpCountLoc, int segmentCountLoc, int segmentIdxLoc, int divisionLoc);

    void RenderPolyline(int colorLoc, int modelLoc, bool grayscale) override;
    bool CreateImgui() override;
    void CreateBsplineImgui() override;
    int addToMG1Scene(MG1::Scene &scene,
                      std::vector<uint32_t> cpsIdxs) override;
};