#include "Intersection.h"
#include <cmath>
#include <algorithm>

Intersection::Intersection(const IntersectionHelpers::IntersectionCurve& curve, int texSize) {
    auto img1 = RasterizeCurveToImage(curve, texSize,
        [](const IntersectionHelpers::IntersectionPoint& p) { return p.uv1; });
    auto img2 = RasterizeCurveToImage(curve, texSize,
        [](const IntersectionHelpers::IntersectionPoint& p) { return p.uv2; });

    texUV1 = CreateOrUpdateTextureRGBA(texUV1, texSize, img1);
    texUV2 = CreateOrUpdateTextureRGBA(texUV2, texSize, img2);

    glm::vec2 avg1 = ComputeAverageUV(curve, [](const auto& p) { return p.uv1; });
    glm::vec2 avg2 = ComputeAverageUV(curve, [](const auto& p) { return p.uv2; });

    int startX, startY;

    // --- uv1 flood-fill ---
    UVtoPixel(avg1, texSize, startX, startY);
    auto img1_flood = img1;
    if (FindFloodFillStart(img1_flood, texSize, texSize, startX, startY)) {
        FloodFill(img1_flood, texSize, texSize, startX, startY, 255, 0, 0, 255);
    }
    texUV1_2 = CreateOrUpdateTextureRGBA(texUV1_2, texSize, img1_flood);

    // --- uv2 flood-fill ---
    UVtoPixel(avg2, texSize, startX, startY);
    auto img2_flood = img2;
    if (FindFloodFillStart(img2_flood, texSize, texSize, startX, startY)) {
        FloodFill(img2_flood, texSize, texSize, startX, startY, 255, 0, 0, 255);
    }
    texUV2_2 = CreateOrUpdateTextureRGBA(texUV2_2, texSize, img2_flood);
}

Intersection::~Intersection() {
    if (texUV1) glDeleteTextures(1, &texUV1);
    if (texUV2) glDeleteTextures(1, &texUV2);
    if (texUV1_2) glDeleteTextures(1, &texUV1_2);
    if (texUV2_2) glDeleteTextures(1, &texUV2_2);
}

#ifdef IMGUI_VERSION
#include <imgui.h>
void Intersection::ShowImGui(int previewSize) const {
    if (ImGui::Begin("UV Textures")) {
        // --- UV1 pair ---
        ImGui::TextUnformatted("uv1:");
        ImGui::Image((ImTextureID)(intptr_t)texUV1,
            ImVec2((float)previewSize, (float)previewSize),
            ImVec2(0, 0), ImVec2(1, 1));
        ImGui::SameLine();
        ImGui::Image((ImTextureID)(intptr_t)texUV1_2,
            ImVec2((float)previewSize, (float)previewSize),
            ImVec2(0, 0), ImVec2(1, 1));

        ImGui::Separator();

        // --- UV2 pair ---
        ImGui::TextUnformatted("uv2:");
        ImGui::Image((ImTextureID)(intptr_t)texUV2,
            ImVec2((float)previewSize, (float)previewSize),
            ImVec2(0, 0), ImVec2(1, 1));
        ImGui::SameLine();
        ImGui::Image((ImTextureID)(intptr_t)texUV2_2,
            ImVec2((float)previewSize, (float)previewSize),
            ImVec2(0, 0), ImVec2(1, 1));
    }
    ImGui::End();
}
#endif


inline void Intersection::PutPixelRGBA(std::vector<uint8_t>& img, int width, int height,
    int x, int y,
    uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (x < 0 || y < 0 || x >= width || y >= height) return;
    const size_t idx = static_cast<size_t>((y * width + x) * 4);
    img[idx + 0] = r;
    img[idx + 1] = g;
    img[idx + 2] = b;
    img[idx + 3] = a;
}

void Intersection::DrawLineBresenham(std::vector<uint8_t>& img, int width, int height,
    int x0, int y0, int x1, int y1,
    uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (x0 == x1 && y0 == y1) {
        PutPixelRGBA(img, width, height, x0, y0, r, g, b, a);
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
            PutPixelRGBA(img, width, height, y, x, r, g, b, a);
        }
        else {
            PutPixelRGBA(img, width, height, x, y, r, g, b, a);
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
    float vf = (1.0f - uv.y) * float(size - 1);
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
        DrawLineBresenham(img, size, size, x0, y0, x1, y1, 255, 255, 255, 255);
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

void Intersection::FloodFill(std::vector<uint8_t>& img, int width, int height,
    int startX, int startY,
    uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    auto getPixel = [&](int x, int y) -> bool {
        if (x < 0 || y < 0 || x >= width || y >= height) return false;
        size_t idx = static_cast<size_t>((y * width + x) * 4);
        return img[idx + 0] == 0 && img[idx + 1] == 0 &&
            img[idx + 2] == 0; // black pixel
        };

    auto setPixel = [&](int x, int y) {
        size_t idx = static_cast<size_t>((y * width + x) * 4);
        img[idx + 0] = r;
        img[idx + 1] = g;
        img[idx + 2] = b;
        img[idx + 3] = a;
        };

    if (!getPixel(startX, startY)) return;

    std::queue<std::pair<int, int>> q;
    q.push({ startX, startY });
    setPixel(startX, startY);

    while (!q.empty()) {
        auto [x, y] = q.front(); q.pop();

        const int dirs[4][2] = { {1,0},{-1,0},{0,1},{0,-1} };
        for (auto& d : dirs) {
            int nx = x + d[0];
            int ny = y + d[1];
            if (getPixel(nx, ny)) {
                setPixel(nx, ny);
                q.push({ nx, ny });
            }
        }
    }
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

    // If already black, fine
    if (isBlack(startX, startY)) return true;

    // Try neighbors in order: left, right, up, down
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

    return false; // No black found nearby
}