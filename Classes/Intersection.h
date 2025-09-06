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
    GLuint texUV1 = 0;
    GLuint texUV2 = 0;
    GLuint texUV1_2 = 0;
    GLuint texUV2_2 = 0;


    Intersection(const IntersectionHelpers::IntersectionCurve& curve, int cpCountLoc, int segmentCountLoc, 
        int segmentIdxLoc, int divisionLoc, int texSize = 1024);
    ~Intersection();

#ifdef IMGUI_VERSION
    void ShowImGui(int previewSize = 256);
#endif

	void Render(int colorLoc, int modelLoc, bool grayscale);
	void RenderPolyline(int colorLoc, int modelLoc, bool grayscale);

private:
    BezierInt bSpline;
    bool showInterpolated = false;

    static inline void PutPixelRGBA(std::vector<uint8_t>& img, int width, int height,
        int x, int y,
        uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

    static void DrawLineBresenham(std::vector<uint8_t>& img, int width, int height,
        int x0, int y0, int x1, int y1,
        uint8_t r = 255, uint8_t g = 255, uint8_t b = 255, uint8_t a = 255);

    static inline int ClampToInt(int v, int lo, int hi);
    static inline void UVtoPixel(const glm::vec2& uv, int size, int& outX, int& outY);

    template <typename UVGetter>
    static std::vector<uint8_t> RasterizeCurveToImage(const IntersectionHelpers::IntersectionCurve& curve,
        int size,
        UVGetter&& getUV);

    static GLuint CreateOrUpdateTextureRGBA(GLuint existingTex,
        int size,
        const std::vector<uint8_t>& rgba);
    int FloodFill(std::vector<uint8_t>& img, int width, int height, int startX, int startY, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    glm::vec2 ComputeAverageUV(const IntersectionHelpers::IntersectionCurve& curve, 
        std::function<glm::vec2(const IntersectionHelpers::IntersectionPoint&)> getUV);
    bool FindFloodFillStart(const std::vector<uint8_t>& img, int width, int height, int& startX, int& startY);
	static void ReverseColors(std::vector<uint8_t>& img);
};
