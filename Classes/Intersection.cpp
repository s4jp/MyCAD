#include "Intersection.h"

#include <cmath>
#include <algorithm>
#include <point.h>
#include <imgui.h>

const glm::vec4 RED(255, 0, 0, 255);
const glm::vec4 BLACK(0, 0, 0, 255);
const glm::vec4 WHITE(255, 255, 255, 255);
const int BIG_TEXTURE_MULTIPLIER = 4;

int Intersection::counter = 0;
void Intersection::ZeroCounter() { counter = 0; }

Intersection::Intersection(const IntersectionHelpers::IntersectionCurve& curve, int cpCountLoc, int segmentCountLoc,
    int segmentIdxLoc, int divisionLoc, int texSize) : bSpline(cpCountLoc, segmentCountLoc, segmentIdxLoc, divisionLoc) {
    fig1 = curve.figA;
	fig2 = curve.figB;
	sameFig = fig1 == fig2;

	TextureCreationLogic(tex1, tex2, curve, texSize);
	TextureCreationLogic(bigTex1, bigTex2, curve, texSize * BIG_TEXTURE_MULTIPLIER);

	std::vector<Figure*> controlPoints;
    for (const auto& p : curve.points) {
		controlPoints.push_back(new Point(p.position, 0.02f, false));
	}
    if (curve.isLoop) {
		controlPoints.push_back(new Point(curve.points.front().position, 0.02f, false));
	}

	bSpline.AddControlPoints(controlPoints);
    bSpline.polylineWidth = 4.0f;
    bSpline.bSpline->curveWidth = 4.0f;
	bSpline.bSpline->usePolyLineColor = true;

    // name = '#' + std::to_string(++counter) + ' ' + fig1->name + " & " + fig2->name;
	name = "Intersection " + std::to_string(++counter);
}

Intersection::~Intersection() {
    if (tex1) glDeleteTextures(1, &tex1);
    if (tex2) glDeleteTextures(1, &tex2);
	if (bigTex1) glDeleteTextures(1, &bigTex1);
	if (bigTex2) glDeleteTextures(1, &bigTex2);
    bSpline.Delete();
}

void Intersection::ShowImGui(int previewSize){
    ImGui::Checkbox("Render", &this->show);
	ImGui::SameLine(ImGui::GetWindowContentRegionMin().x + ImGui::GetWindowContentRegionMax().x * 0.5f);
    ImGui::Checkbox("Bspline mode", &this->showInterpolated);

    ImGui::SeparatorText(("%s options:",fig1->name).c_str());
    if (sameFig && (fig1hasRed || fig2hasRed))
		ImGui::Checkbox("Use first texture", &useFirstTexture);
    if (fig1hasRed && (!sameFig || (sameFig && useFirstTexture))) {
        ImGui::Checkbox("Hide red", &tex1_hideRed);
        ImGui::SameLine(ImGui::GetWindowContentRegionMin().x + ImGui::GetWindowContentRegionMax().x * 0.5f);
	    ImGui::Checkbox("Hide black", &tex1_hideBlack);
    }

    ImGui::Image((ImTextureID)(intptr_t)tex1,
        ImVec2((float)previewSize, (float)previewSize),
        ImVec2(0,1), ImVec2(1,0));

    if (!sameFig)
        ImGui::SeparatorText(("%s options:",fig2->name).c_str());
    if (fig2hasRed && (!sameFig || (sameFig && !useFirstTexture))) {
        ImGui::Checkbox("Hide red ", &tex2_hideRed);
        ImGui::SameLine(ImGui::GetWindowContentRegionMin().x + ImGui::GetWindowContentRegionMax().x * 0.5f);
	    ImGui::Checkbox("Hide black ", &tex2_hideBlack);
    }

    ImGui::Image((ImTextureID)(intptr_t)tex2,
        ImVec2((float)previewSize, (float)previewSize),
        ImVec2(0,1), ImVec2(1,0));
}

inline void Intersection::PutWhitePixel(std::vector<uint8_t>& img, int width, int height, int x, int y) {
    if (x < 0 || y < 0 || x >= width || y >= height) return;
    const size_t idx = static_cast<size_t>((y * width + x) * 4);
    img[idx + 0] = WHITE.x;
    img[idx + 1] = WHITE.y;
    img[idx + 2] = WHITE.z;
    img[idx + 3] = WHITE.w;
}

void Intersection::DrawLineBresenham(std::vector<uint8_t>& img, int width, int height, int x0, int y0, int x1, int y1) {
    if (x0 == x1 && y0 == y1) {
        PutWhitePixel(img, width, height, x0, y0);
        return;
    }

    const bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);
    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = std::abs(y1 - y0);
    int err = dx / 2;
    int ystep = (y0 < y1) ? 1 : -1;

    int y = y0;
    for (int x = x0; x <= x1; ++x) {
        if (steep) {
            PutWhitePixel(img, width, height, y, x);
        }
        else {
            PutWhitePixel(img, width, height, x, y);
        }
        err -= dy;
        if (err < 0) {
            y += ystep;
            err += dx;
        }
    }
}

inline int Intersection::ClampToInt(int v, int lo, int hi) {
    return std::max(lo, std::min(v, hi));
}

inline void Intersection::UVtoPixel(const glm::vec2& uv, int size, int& outX, int& outY) {
    float uf = uv.x * float(size - 1);
    float vf = uv.y * float(size - 1);
    outX = ClampToInt(int(std::round(uf)), 0, size - 1);
    outY = ClampToInt(int(std::round(vf)), 0, size - 1);
}

template <typename UVGetter>
std::vector<uint8_t> Intersection::RasterizeCurveToImage(
    const IntersectionHelpers::IntersectionCurve& curve,
    int size,
    UVGetter&& getUV)
{
    std::vector<uint8_t> img(static_cast<size_t>(size * size * 4), 0);
    for (size_t i = 0; i < img.size(); i += 4)
        img[i + 3] = 255;

    if (curve.points.empty()) return img;

    using Polyline = std::vector<glm::vec2>;
    std::vector<Polyline> subCurves;

    Polyline current;
    current.push_back(getUV(curve.points[0]));

    const size_t n = curve.isLoop ? curve.points.size() + 1 : curve.points.size();
    for (size_t i = 1; i < n; ++i) {
        glm::vec2 p0 = getUV(curve.points[i - 1]);
        glm::vec2 p1 = getUV(curve.points[i % curve.points.size()]);
        glm::vec2 d = p1 - p0;

        bool wrapU = std::abs(d.x) > 0.5f;
        bool wrapV = std::abs(d.y) > 0.5f;

        if (wrapU || wrapV) {
            if (wrapU) {
                float t = (d.x > 0) ? (1.0f - p0.x) / d.x : (0.0f - p0.x) / d.x;
                t = std::clamp(t, 0.0f, 1.0f);
                glm::vec2 mid = p0 + t * d;

                glm::vec2 edge1 = { (d.x > 0) ? 0.0f : 1.0f, mid.y };
                glm::vec2 edge2 = { (d.x > 0) ? 1.0f : 0.0f, mid.y };

                current.push_back(edge1);
                subCurves.push_back(current);

                current.clear();
                current.push_back(edge2);
                current.push_back(p1);
            }
            else if (wrapV) {
                float t = (d.y > 0) ? (1.0f - p0.y) / d.y : (0.0f - p0.y) / d.y;
                t = std::clamp(t, 0.0f, 1.0f);
                glm::vec2 mid = p0 + t * d;

                glm::vec2 edge1 = { mid.x, (d.y > 0) ? 0.0f : 1.0f };
                glm::vec2 edge2 = { mid.x, (d.y > 0) ? 1.0f : 0.0f };

                current.push_back(edge1);
                subCurves.push_back(current);

                current.clear();
                current.push_back(edge2);
                current.push_back(p1);
            }
        }
        else {
            current.push_back(p1);
        }
    }

    if (!current.empty())
        subCurves.push_back(current);

    auto drawSegment = [&](const glm::vec2& a, const glm::vec2& b) {
        int x0, y0, x1, y1;
        UVtoPixel(a, size, x0, y0);
        UVtoPixel(b, size, x1, y1);
        DrawLineBresenham(img, size, size, x0, y0, x1, y1);
        };

    for (const auto& sub : subCurves) {
        for (size_t i = 0; i + 1 < sub.size(); ++i) {
            drawSegment(sub[i], sub[i + 1]);
        }
    }

    return img;
}

GLuint Intersection::CreateOrUpdateTextureRGBA(GLuint existingTex,
    int size,
    const std::vector<uint8_t>& rgba) {
    GLuint tex = existingTex;
    if (tex == 0) {
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size, size, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
    }
    else {
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, size,
            GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}

int Intersection::FloodFill(std::vector<uint8_t>& img, int width, int height, int startX, int startY, bool uWrapped, bool vWrapped)
{
    auto getPixel = [&](int x, int y) -> bool {
        if (x < 0 || y < 0 || x >= width || y >= height) return false;
        size_t idx = static_cast<size_t>((y * width + x) * 4);
        return img[idx + 0] == BLACK.x && img[idx + 1] == BLACK.y && img[idx + 2] == BLACK.z;
        };

    auto setPixel = [&](int x, int y) {
        size_t idx = static_cast<size_t>((y * width + x) * 4);
        img[idx + 0] = RED.x;
        img[idx + 1] = RED.y;
        img[idx + 2] = RED.z;
        img[idx + 3] = RED.w;
        };

    auto wrapIfApplicable = [&](int& x, int& y) {
        if (uWrapped)
            x = (x + width) % width;
		if (vWrapped)
			y = (y + height) % height;
		};

    if (!getPixel(startX, startY)) return 0;

    int count = 1;
    std::queue<std::pair<int, int>> q;
    q.push({ startX, startY });
    setPixel(startX, startY);

    while (!q.empty()) {
        auto [x, y] = q.front(); q.pop();

        const int dirs[4][2] = { {1,0},{-1,0},{0,1},{0,-1} };
        for (auto& d : dirs) {
            int nx = x + d[0];
            int ny = y + d[1];
			wrapIfApplicable(nx, ny);
            if (getPixel(nx, ny)) {
                setPixel(nx, ny);
                q.push({ nx, ny });
                count++;
            }
        }
    }

	return count;
}

glm::vec2 Intersection::ComputeAverageUV(
    const IntersectionHelpers::IntersectionCurve& curve,
    std::function<glm::vec2(const IntersectionHelpers::IntersectionPoint&)> getUV)
{
    if (curve.points.empty()) return glm::vec2(0.5f, 0.5f);

    glm::vec2 sum(0.0f);
    for (auto& p : curve.points) {
        sum += getUV(p);
    }
    return sum / float(curve.points.size());
}

bool Intersection::FindFloodFillStart(
    const std::vector<uint8_t>& img,
    int width, int height,
    int& startX, int& startY)
{
    auto isBlack = [&](int x, int y) {
        if (x < 0 || y < 0 || x >= width || y >= height) return false;
        size_t idx = static_cast<size_t>((y * width + x) * 4);
        return img[idx + 0] == 0 && img[idx + 1] == 0 && img[idx + 2] == 0;
        };

    if (isBlack(startX, startY)) return true;

    const int dirs[4][2] = { {-1,0}, {1,0}, {0,-1}, {0,1} };
    for (auto& d : dirs) {
        int nx = startX + d[0];
        int ny = startY + d[1];
        if (isBlack(nx, ny)) {
            startX = nx;
            startY = ny;
            return true;
        }
    }

    return false;
}

int Intersection::ReverseColors(std::vector<uint8_t>& img)
{
	int count = 0;

    for (size_t i = 0; i < img.size(); i += 4) {
        bool isRed = (img[i + 0] == RED.x && img[i + 1] == RED.y && img[i + 2] == RED.z);
        bool isBlack = (img[i + 0] == BLACK.x && img[i + 1] == BLACK.y && img[i + 2] == BLACK.z);
        if (isRed) {
            img[i + 0] = BLACK.x; img[i + 1] = BLACK.y; img[i + 2] = BLACK.z;
        }
        else if (isBlack) {
            img[i + 0] = RED.x; img[i + 1] = RED.y; img[i + 2] = RED.z;
            count++;
        }
    }

	return count;
}

void Intersection::TextureCreationLogic(GLuint& texture1, GLuint& texture2, const IntersectionHelpers::IntersectionCurve& curve, int texSize)
{
    auto img1 = RasterizeCurveToImage(curve, texSize,
        [](const IntersectionHelpers::IntersectionPoint& p) { return p.uv1; });
    auto img2 = RasterizeCurveToImage(curve, texSize,
        [](const IntersectionHelpers::IntersectionPoint& p) { return p.uv2; });

    glm::vec2 avg1 = ComputeAverageUV(curve, [](const auto& p) { return p.uv1; });
    glm::vec2 avg2 = ComputeAverageUV(curve, [](const auto& p) { return p.uv2; });

    int startX, startY;
    UVtoPixel(avg1, texSize, startX, startY);
    auto img1_flood = img1;
    int count1 = 0;
    if (FindFloodFillStart(img1_flood, texSize, texSize, startX, startY)) {
        count1 = FloodFill(img1_flood, texSize, texSize, startX, startY, fig1->IsWrappedU(), fig1->IsWrappedV());
    }

    UVtoPixel(avg2, texSize, startX, startY);
    auto img2_flood = img2;
    int count2 = 0;
    if (FindFloodFillStart(img2_flood, texSize, texSize, startX, startY)) {
        count2 = FloodFill(img2_flood, texSize, texSize, startX, startY, fig2->IsWrappedU(), fig2->IsWrappedV());
    }

    bool img1toFix = false;
    bool img2toFix = false;
    if (count1 > 0.5 * texSize * texSize && count2 > 0.5 * texSize * texSize) {
        img1toFix = true;
        img2toFix = true;
    }
    else if (std::abs(count1 - count2) > (texSize * texSize / 10)) {
        if (count1 > count2) {
            img1toFix = true;
        }
        else {
            img2toFix = true;
        }
    }

	if (img1toFix) count1 = ReverseColors(img1_flood);
	if (img2toFix) count2 = ReverseColors(img2_flood);

	fig1hasRed = count1 > 0;
	fig2hasRed = count2 > 0;

    texture1 = CreateOrUpdateTextureRGBA(texture1, texSize, img1_flood);
    texture2 = CreateOrUpdateTextureRGBA(texture2, texSize, img2_flood);
}

void Intersection::Render(int colorLoc, int modelLoc, bool grayscale) {
    if (showInterpolated && show) bSpline.Render(colorLoc, modelLoc, grayscale);
}

void Intersection::RenderPolyline(int colorLoc, int modelLoc, bool grayscale) {
    if (!showInterpolated && show) bSpline.RenderPolyline(colorLoc, modelLoc, grayscale);
}