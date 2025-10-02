#pragma once

#include <vector>
#include <cstdint>
#include <glad/glad.h>
#include <IntersectionHelpers.h>
#include <queue>
#include <functional>
#include <bezierInt.h>

class Intersection {
public:
    Figure* fig1 = nullptr;
	Figure* fig2 = nullptr;

    GLuint bigTex1;
    GLuint bigTex2;

	bool tex1_hideRed = false;
	bool tex1_hideBlack = false;
	bool tex2_hideRed = false;
	bool tex2_hideBlack = false;

	bool selected = false;
	std::string name;

    bool sameFig;
	bool useFirstTexture = true;

    Intersection(const IntersectionHelpers::IntersectionCurve& curve, int cpCountLoc, int segmentCountLoc, 
        int segmentIdxLoc, int divisionLoc, int texSize);
    ~Intersection();

    void ShowImGui(int previewSize);

	void Render(int colorLoc, int modelLoc, bool grayscale);
	void RenderPolyline(int colorLoc, int modelLoc, bool grayscale);

    static void ZeroCounter();

private:
    GLuint tex1;
    GLuint tex2;

    BezierInt bSpline;
    bool showInterpolated = false;
	bool show = true;

    bool fig1hasRed;
    bool fig2hasRed;

    static int counter;

    static inline void PutWhitePixel(std::vector<uint8_t>& img, int width, int height, int x, int y);
    static void DrawLineBresenham(std::vector<uint8_t>& img, int width, int height, int x0, int y0, int x1, int y1);

    static inline int ClampToInt(int v, int lo, int hi);
    static inline void UVtoPixel(const glm::vec2& uv, int size, int& outX, int& outY);

    template <typename UVGetter>
    static std::vector<uint8_t> RasterizeCurveToImage(const IntersectionHelpers::IntersectionCurve& curve, int size, UVGetter&& getUV);

    static GLuint CreateOrUpdateTextureRGBA(GLuint existingTex, int size, const std::vector<uint8_t>& rgba);
    int FloodFill(std::vector<uint8_t>& img, int width, int height, int startX, int startY, bool uWrapped, bool vWrapped);
    glm::vec2 ComputeAverageUV(const IntersectionHelpers::IntersectionCurve& curve, std::function<glm::vec2(const IntersectionHelpers::IntersectionPoint&)> getUV);
    bool FindFloodFillStart(const std::vector<uint8_t>& img, int width, int height, int& startX, int& startY);
	static int ReverseColors(std::vector<uint8_t>& img);
    void TextureCreationLogic(GLuint& texture1, GLuint& texture2, const IntersectionHelpers::IntersectionCurve& curve, int texSize);
};
