#include <SDL.h>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <cmath>

#include <SDL2_gfxPrimitives.h> // Xu ly rang cua

struct Point { // cai dat toa do diem trong khong gian 2D
    float x, y;
};

Point operator+(const Point& a, const Point& b) { return { a.x + b.x, a.y + b.y }; }     // cong toa do
Point operator-(const Point& a, const Point& b) { return { a.x - b.x, a.y - b.y }; }     // tru toa do
Point operator*(const Point& p, float scalar) { return { p.x * scalar, p.y * scalar }; } // ti le

struct BezierPath {
    Point p0, p1, p2, p3; // Start, Control1, Control2, End
};

// Helper to create a straight line using BezierPath structure
BezierPath make_line(Point start, Point end) {
    return { start, start, end, end };
}

struct Glyph {
    char character_code;
    std::vector<BezierPath> paths;
    float advanceWidth;
    Point minBounds, maxBounds;

    Glyph() : character_code(0), advanceWidth(0.0f), minBounds({ 0,0 }), maxBounds({ 0,0 }) {}

    static Point getCubicBezierPoint(Point p0, Point p1, Point p2, Point p3, float t) {
        Point result;
        float u = 1.0f - t;
        float tt = t * t;
        float uu = u * u;
        float uuu = uu * u;
        float ttt = tt * t;
        result.x = uuu * p0.x + 3.0f * uu * t * p1.x + 3.0f * u * tt * p2.x + ttt * p3.x;
        result.y = uuu * p0.y + 3.0f * uu * t * p1.y + 3.0f * u * tt * p2.y + ttt * p3.y;
        return result;
    }

    void calculateBounds() {
        if (paths.empty()) {
            minBounds = { 0,0 }; maxBounds = { 0,0 };
            if (character_code == ' ' && advanceWidth > 0) { // Special handling for space
                minBounds = { 0, -1 }; // Give it some minimal height for layout consistency
                maxBounds = { advanceWidth, 1 };
            }
            return;
        }
        minBounds = paths[0].p0;
        maxBounds = paths[0].p0;

        auto update_bounds_for_point = [&](const Point& p) {
            minBounds.x = std::min(minBounds.x, p.x);
            minBounds.y = std::min(minBounds.y, p.y);
            maxBounds.x = std::max(maxBounds.x, p.x);
            maxBounds.y = std::max(maxBounds.y, p.y);
            };

        for (const auto& path : paths) {
            const int samples = 20;
            update_bounds_for_point(path.p0);
            for (int i = 1; i <= samples; ++i) {
                float t = static_cast<float>(i) / static_cast<float>(samples);
                Point curvePoint = getCubicBezierPoint(path.p0, path.p1, path.p2, path.p3, t);
                update_bounds_for_point(curvePoint);
            }
            update_bounds_for_point(path.p3);
        }
    }

    Point getSize() const {
        return maxBounds - minBounds;
    }
};

// --- Rendering Function ---
void renderTransformedGlyph_AntiAliased(SDL_Renderer* renderer, const Glyph& glyph,
    float scale, Point screenTopLeftOfBoundingBox,
    SDL_Color color, int segmentsPerCurve, Uint8 thickness) {
    if (glyph.paths.empty() || thickness == 0) return;

    for (const auto& path : glyph.paths) {
        BezierPath screenPath = {
            (path.p0 - glyph.minBounds) * scale + screenTopLeftOfBoundingBox,
            (path.p1 - glyph.minBounds) * scale + screenTopLeftOfBoundingBox,
            (path.p2 - glyph.minBounds) * scale + screenTopLeftOfBoundingBox,
            (path.p3 - glyph.minBounds) * scale + screenTopLeftOfBoundingBox
        };

        Point prevPoint = screenPath.p0;
        for (int i = 1; i <= segmentsPerCurve; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(segmentsPerCurve);
            Point currentPoint = Glyph::getCubicBezierPoint(screenPath.p0, screenPath.p1, screenPath.p2, screenPath.p3, t);

            thickLineRGBA(renderer,
                static_cast<Sint16>(prevPoint.x), static_cast<Sint16>(prevPoint.y),
                static_cast<Sint16>(currentPoint.x), static_cast<Sint16>(currentPoint.y),
                thickness,
                color.r, color.g, color.b, color.a);

            prevPoint = currentPoint;
        }
    }
}


// --- Glyph Definition Functions ---
const float GLYPH_DESIGN_HEIGHT = 40.0f;
const float GLYPH_DESIGN_WIDTH = 30.0f;

Glyph createVectorGlyphN() {
    Glyph n; n.character_code = 'N';
    float w = GLYPH_DESIGN_WIDTH * 0.9f; float h = GLYPH_DESIGN_HEIGHT;
    Point p0 = { 0, h }; Point p1 = { 0, 0 }; Point p2 = { w, h }; Point p3 = { w, 0 };
    n.paths.push_back(make_line(p0, p1));
    n.paths.push_back(make_line(p1, p2));
    n.paths.push_back(make_line(p2, p3));
    n.advanceWidth = w + 5.0f; n.calculateBounds(); return n;
}

Glyph createVectorGlyphO() {
    Glyph o; o.character_code = 'O';
    float cx = GLYPH_DESIGN_WIDTH / 2.0f; float cy = GLYPH_DESIGN_HEIGHT / 2.0f;
    float rx = GLYPH_DESIGN_WIDTH / 2.0f; float ry = GLYPH_DESIGN_HEIGHT / 2.0f;
    float c_bez = 0.552284749831f; // Bezier control point constant for a circle approximation
    o.paths.push_back({ {cx, cy - ry}, {cx + rx * c_bez, cy - ry}, {cx + rx, cy - ry * c_bez}, {cx + rx, cy} });
    o.paths.push_back({ {cx + rx, cy}, {cx + rx, cy + ry * c_bez}, {cx + rx * c_bez, cy + ry}, {cx, cy + ry} });
    o.paths.push_back({ {cx, cy + ry}, {cx - rx * c_bez, cy + ry}, {cx - rx, cy + ry * c_bez}, {cx - rx, cy} });
    o.paths.push_back({ {cx - rx, cy}, {cx - rx, cy - ry * c_bez}, {cx - rx * c_bez, cy - ry}, {cx, cy - ry} });
    o.advanceWidth = GLYPH_DESIGN_WIDTH + 5.0f; o.calculateBounds(); return o;
}

Glyph createVectorGlyphP() {
    Glyph p_glyph; p_glyph.character_code = 'P'; // Renamed to avoid conflict
    float h = GLYPH_DESIGN_HEIGHT; float w_head = GLYPH_DESIGN_WIDTH * 0.9f;
    float h_head_top = 0; float h_head_bottom = GLYPH_DESIGN_HEIGHT / 2.0f;
    Point stem_top = { 0,0 }; Point stem_bottom = { 0,h };
    Point head_top_left = { 0, h_head_top };
    Point head_top_right_ctrl = { w_head * 1.3f, h_head_top - h_head_bottom * 0.1f };
    Point head_bottom_right_ctrl = { w_head * 1.3f, h_head_bottom + h_head_bottom * 0.1f };
    Point head_bottom_left = { 0, h_head_bottom };

    p_glyph.paths.push_back(make_line(stem_top, stem_bottom));
    p_glyph.paths.push_back({ head_top_left, head_top_right_ctrl, head_bottom_right_ctrl, head_bottom_left });
    p_glyph.advanceWidth = w_head + 2.0f; p_glyph.calculateBounds(); return p_glyph;
}

Glyph createVectorGlyphQ() {
    Glyph q_glyph; q_glyph.character_code = 'Q'; // Renamed to avoid conflict
    float cx = GLYPH_DESIGN_WIDTH / 2.0f; float cy = GLYPH_DESIGN_HEIGHT / 2.0f;
    float rx = GLYPH_DESIGN_WIDTH / 2.0f; float ry = GLYPH_DESIGN_HEIGHT / 2.0f;
    float c_bez = 0.552284749831f;
    q_glyph.paths.push_back({ {cx, cy - ry}, {cx + rx * c_bez, cy - ry}, {cx + rx, cy - ry * c_bez}, {cx + rx, cy} });
    q_glyph.paths.push_back({ {cx + rx, cy}, {cx + rx, cy + ry * c_bez}, {cx + rx * c_bez, cy + ry}, {cx, cy + ry} });
    q_glyph.paths.push_back({ {cx, cy + ry}, {cx - rx * c_bez, cy + ry}, {cx - rx, cy + ry * c_bez}, {cx - rx, cy} });
    q_glyph.paths.push_back({ {cx - rx, cy}, {cx - rx, cy - ry * c_bez}, {cx - rx * c_bez, cy - ry}, {cx, cy - ry} });
    Point tail_start = { cx + rx * 0.5f, cy + ry * 0.5f };
    Point tail_end = { cx + rx + 6.0f, cy + ry + 6.0f };
    q_glyph.paths.push_back(make_line(tail_start, tail_end));
    q_glyph.advanceWidth = GLYPH_DESIGN_WIDTH + 10.0f; q_glyph.calculateBounds(); return q_glyph;
}

Glyph createVectorGlyphR() {
    Glyph r_glyph; r_glyph.character_code = 'R'; // Renamed to avoid conflict
    float h = GLYPH_DESIGN_HEIGHT; float w_head = GLYPH_DESIGN_WIDTH * 0.9f;
    float h_head_top = 0; float h_head_bottom = GLYPH_DESIGN_HEIGHT / 2.0f;
    Point stem_top = { 0,0 }; Point stem_bottom = { 0,h };
    Point head_top_left = { 0, h_head_top };
    Point head_top_right_ctrl = { w_head * 1.3f, h_head_top - h_head_bottom * 0.1f };
    Point head_bottom_right_ctrl = { w_head * 1.3f, h_head_bottom + h_head_bottom * 0.1f };
    Point head_bottom_left = { 0, h_head_bottom };

    r_glyph.paths.push_back(make_line(stem_top, stem_bottom));
    r_glyph.paths.push_back({ head_top_left, head_top_right_ctrl, head_bottom_right_ctrl, head_bottom_left });
    Point leg_start = { w_head * 0.4f, h_head_bottom };
    Point leg_end = { w_head * 1.0f, h };
    r_glyph.paths.push_back(make_line(leg_start, leg_end));
    r_glyph.advanceWidth = w_head + 5.0f; r_glyph.calculateBounds(); return r_glyph;
}

// =======================================================================================
// HÀM ĐƯỢC KHÔI PHỤC THEO ĐIỂM CỦA NGƯỜI DÙNG
// =======================================================================================
Glyph createVectorGlyphS() {
    Glyph s; s.character_code = 'S';
    float w = GLYPH_DESIGN_WIDTH * 0.9f;
    float h = GLYPH_DESIGN_HEIGHT;

    // Sử dụng bộ điểm "Further refinement" từ code gốc của bạn
    Point p0, p1, p2, p3, p4, p5, p6, p7;

    p0 = { w * 0.8f, h * 0.15f };
    p1 = { w * 1.1f, h * 0.05f }; // Pull top curve more to the right initially
    p2 = { w * 0.2f, h * 0.35f }; // Bring first curve's end towards center-left
    p3 = { w * 0.5f, h * 0.5f };  // Center inflection point

    p4 = p3; // Start of bottom curve is the end of the top curve
    p5 = { w * 0.8f, h * 0.65f }; // Bring second curve's start from center-right
    p6 = { -w * 0.1f, h * 0.95f }; // Pull bottom curve more to the left at the end
    p7 = { w * 0.2f, h * 0.85f };

    s.paths.push_back({ p0, p1, p2, p3 });
    s.paths.push_back({ p4, p5, p6, p7 });

    s.advanceWidth = w + 2.0f; // Giữ nguyên advanceWidth từ code gốc của bạn cho chữ S
    s.calculateBounds();
    return s;
}
// =======================================================================================
// KẾT THÚC HÀM KHÔI PHỤC
// =======================================================================================

Glyph createVectorGlyphT() {
    Glyph t; t.character_code = 'T';
    float w = GLYPH_DESIGN_WIDTH; float h = GLYPH_DESIGN_HEIGHT;
    t.paths.push_back(make_line({ w / 2.0f, 0 }, { w / 2.0f, h }));
    t.paths.push_back(make_line({ 0, 0 }, { w, 0 }));
    t.advanceWidth = w + 5.0f; t.calculateBounds(); return t;
}

Glyph createVectorGlyphU() {
    Glyph u; u.character_code = 'U';
    float w = GLYPH_DESIGN_WIDTH * 0.9f; float h = GLYPH_DESIGN_HEIGHT; float r = w / 2.0f;
    Point ul = { 0,0 }; Point ur = { w,0 };
    Point bl = { 0, h - r }; Point br = { w, h - r };
    u.paths.push_back(make_line(ul, bl));
    u.paths.push_back({ bl, {bl.x, bl.y + r * 1.2f} , {br.x, br.y + r * 1.2f}, br });
    u.paths.push_back(make_line(br, ur));
    u.advanceWidth = w + 5.0f; u.calculateBounds(); return u;
}

Glyph createVectorGlyphV() {
    Glyph v; v.character_code = 'V';
    float w = GLYPH_DESIGN_WIDTH; float h = GLYPH_DESIGN_HEIGHT;
    v.paths.push_back(make_line({ 0,0 }, { w / 2.0f, h }));
    v.paths.push_back(make_line({ w / 2.0f, h }, { w,0 }));
    v.advanceWidth = w + 5.0f; v.calculateBounds(); return v;
}

Glyph createVectorGlyphW() {
    Glyph w_glyph; w_glyph.character_code = 'W';
    float w = GLYPH_DESIGN_WIDTH * 1.3f; float h = GLYPH_DESIGN_HEIGHT;
    Point p0 = { 0,0 }; Point p1 = { w * 0.25f, h }; Point p2 = { w * 0.5f, h * 0.3f };
    Point p3 = { w * 0.75f, h }; Point p4 = { w,0 };
    w_glyph.paths.push_back(make_line(p0, p1));
    w_glyph.paths.push_back(make_line(p1, p2));
    w_glyph.paths.push_back(make_line(p2, p3));
    w_glyph.paths.push_back(make_line(p3, p4));
    w_glyph.advanceWidth = w + 5.0f; w_glyph.calculateBounds(); return w_glyph;
}

Glyph createVectorGlyphX() {
    Glyph x_glyph; x_glyph.character_code = 'X';
    float w = GLYPH_DESIGN_WIDTH * 0.9f; float h = GLYPH_DESIGN_HEIGHT;
    x_glyph.paths.push_back(make_line({ 0,0 }, { w,h }));
    x_glyph.paths.push_back(make_line({ w,0 }, { 0,h }));
    x_glyph.advanceWidth = w + 5.0f; x_glyph.calculateBounds(); return x_glyph;
}

Glyph createVectorGlyphY() {
    Glyph y_glyph; y_glyph.character_code = 'Y';
    float w = GLYPH_DESIGN_WIDTH; float h = GLYPH_DESIGN_HEIGHT;
    Point top_left = { 0,0 }; Point top_right = { w,0 };
    Point middle_join = { w / 2.0f, h / 2.0f };
    Point bottom_stem = { w / 2.0f, h };
    y_glyph.paths.push_back(make_line(top_left, middle_join));
    y_glyph.paths.push_back(make_line(top_right, middle_join));
    y_glyph.paths.push_back(make_line(middle_join, bottom_stem));
    y_glyph.advanceWidth = w + 5.0f; y_glyph.calculateBounds(); return y_glyph;
}

Glyph createVectorGlyphZ() {
    Glyph z_glyph; z_glyph.character_code = 'Z';
    float w = GLYPH_DESIGN_WIDTH * 0.9f; float h = GLYPH_DESIGN_HEIGHT;
    z_glyph.paths.push_back(make_line({ 0,0 }, { w,0 }));
    z_glyph.paths.push_back(make_line({ w,0 }, { 0,h }));
    z_glyph.paths.push_back(make_line({ 0,h }, { w,h }));
    z_glyph.advanceWidth = w + 5.0f; z_glyph.calculateBounds(); return z_glyph;
}


// --- SDL Initialization and Main Loop ---
SDL_Window* gWindow_alph = nullptr;
SDL_Renderer* gRenderer_alph = nullptr;

bool initSDL_Alphabet() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) { std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl; return false; }
    gWindow_alph = SDL_CreateWindow("Vector Alphabet Demo (SDL2_gfx)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1200, 800, SDL_WINDOW_SHOWN);
    if (!gWindow_alph) { std::cerr << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl; return false; }
    gRenderer_alph = SDL_CreateRenderer(gWindow_alph, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!gRenderer_alph) { std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl; return false; }
    return true;
}

void closeSDL_Alphabet() {
    SDL_DestroyRenderer(gRenderer_alph);
    SDL_DestroyWindow(gWindow_alph);
    gWindow_alph = nullptr; gRenderer_alph = nullptr;
    SDL_Quit();
}

int main(int argc, char* args[]) {
    if (!initSDL_Alphabet()) { std::cerr << "Failed to initialize SDL!" << std::endl; return -1; }

    std::vector<Glyph> alphabet;
    alphabet.push_back(createVectorGlyphN());
    alphabet.push_back(createVectorGlyphO());
    alphabet.push_back(createVectorGlyphP());
    alphabet.push_back(createVectorGlyphQ());
    alphabet.push_back(createVectorGlyphR());
    alphabet.push_back(createVectorGlyphS());
    alphabet.push_back(createVectorGlyphT());
    alphabet.push_back(createVectorGlyphU());
    alphabet.push_back(createVectorGlyphV());
    alphabet.push_back(createVectorGlyphW());
    alphabet.push_back(createVectorGlyphX());
    alphabet.push_back(createVectorGlyphY());
    alphabet.push_back(createVectorGlyphZ());

    bool quit = false;
    SDL_Event e;
    float displayScale = 4.0f;
    SDL_Color drawingColor = { 30, 30, 30, 255 };
    Uint8 lineThickness = 3;

    SDL_SetRenderDrawBlendMode(gRenderer_alph, SDL_BLENDMODE_BLEND);

    Point currentDisplayPos = { 30, 60 };
    float lineSpacingFactor = 1.5f;
    float letterSpacing = 5.0f * displayScale;


    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(gRenderer_alph, &screenWidth, &screenHeight);
    float screenRenderWidth = static_cast<float>(screenWidth - currentDisplayPos.x);


    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) quit = true;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_UP) {
                    lineThickness = std::min((Uint8)10, (Uint8)(lineThickness + 1));
                }
                else if (e.key.keysym.sym == SDLK_DOWN) {
                    lineThickness = std::max((Uint8)1, (Uint8)(lineThickness - 1));
                }
                else if (e.key.keysym.sym == SDLK_LEFT) {
                    displayScale = std::max(0.5f, displayScale - 0.2f);
                    letterSpacing = 5.0f * displayScale;

                }
                else if (e.key.keysym.sym == SDLK_RIGHT) {
                    displayScale = std::min(10.0f, displayScale + 0.2f);
                    letterSpacing = 5.0f * displayScale;
                }
            }
        }

        SDL_SetRenderDrawColor(gRenderer_alph, 245, 245, 245, 255);
        SDL_RenderClear(gRenderer_alph);

        Point renderPos = currentDisplayPos;
        float currentLineMaxHeight = 0.0f;


        for (const auto& glyph : alphabet) {
            if (glyph.paths.empty() && glyph.character_code != ' ') continue;

            Point glyphOriginalSize = glyph.getSize();
            float glyphDisplayWidth = glyph.advanceWidth * displayScale;
            // Sử dụng kích thước thực tế của glyph để tính chiều cao hiển thị, thay vì GLYPH_DESIGN_HEIGHT
            // Điều này quan trọng vì calculateBounds() sẽ cho chiều cao thực tế của từng glyph
            float glyphDisplayHeight = (glyph.maxBounds.y - glyph.minBounds.y) * displayScale;
            if (glyphDisplayHeight <= 0 && glyph.character_code != ' ') { // Xử lý trường hợp glyph không có chiều cao (ví dụ: chỉ có đường ngang)
                glyphDisplayHeight = GLYPH_DESIGN_HEIGHT * displayScale * 0.1f; // Gán một chiều cao nhỏ để không làm hỏng layout
            }


            if (renderPos.x + glyphDisplayWidth > screenRenderWidth && renderPos.x > currentDisplayPos.x) {
                renderPos.x = currentDisplayPos.x;
                renderPos.y += currentLineMaxHeight * lineSpacingFactor;
                currentLineMaxHeight = 0;
            }

            currentLineMaxHeight = std::max(currentLineMaxHeight, glyphDisplayHeight);


            Point topLeftOfBoundingBox = renderPos;
            // Không cần điều chỉnh Y dựa trên maxBounds nữa nếu calculateBounds hoạt động chính xác
            // và các điểm được định nghĩa với gốc (0,0) ở góc trên bên trái của bounding box thiết kế.
            // Tuy nhiên, nếu bạn muốn các ký tự căn chỉnh theo một đường baseline cụ thể,
            // bạn có thể cần tính toán offset dựa trên minBounds.y hoặc maxBounds.y
            // Ví dụ, để căn theo đáy:
            // topLeftOfBoundingBox.y += (GLYPH_DESIGN_HEIGHT - glyph.maxBounds.y) * displayScale; 
            // Hoặc căn theo đỉnh:
            // topLeftOfBoundingBox.y -= glyph.minBounds.y * displayScale;


            renderTransformedGlyph_AntiAliased(gRenderer_alph, glyph, displayScale, topLeftOfBoundingBox, drawingColor, 30 /*segments*/, lineThickness);
            renderPos.x += glyphDisplayWidth + letterSpacing;
        }

        SDL_RenderPresent(gRenderer_alph);
    }

    closeSDL_Alphabet();
    return 0;
}
