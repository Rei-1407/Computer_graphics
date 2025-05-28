#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <iostream>
#include <cmath>
#include <vector>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;

const int GRID_CELL_SIZE = 10;
const int PIXEL_ART_OFFSET_X = 0;
const int PIXEL_ART_OFFSET_Y = 0;
const int CANVAS_WIDTH_CELLS = SCREEN_WIDTH / GRID_CELL_SIZE;
const int CANVAS_HEIGHT_CELLS = SCREEN_HEIGHT / GRID_CELL_SIZE;
const int CANVAS_SCREEN_WIDTH = CANVAS_WIDTH_CELLS * GRID_CELL_SIZE;
const int CANVAS_SCREEN_HEIGHT = CANVAS_HEIGHT_CELLS * GRID_CELL_SIZE;

std::vector<std::vector<SDL_Color>> pixelCanvas;
SDL_Color currentDrawingColor = { 0, 0, 0, 255 };
SDL_Color canvasBackgroundColor = { 220, 220, 220, 255 };

bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    gWindow = SDL_CreateWindow("SDL Fullscreen Pixel Art with N Frame (Aligned)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (gRenderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }
    pixelCanvas.resize(CANVAS_HEIGHT_CELLS, std::vector<SDL_Color>(CANVAS_WIDTH_CELLS, canvasBackgroundColor));
    return true;
}

void closeSDL() {
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;
    gRenderer = nullptr;
    SDL_Quit();
}

void drawHollowA(SDL_Renderer* renderer, int x, int y, int W, int H, int T, SDL_Color color, int sdlOutlineThickness) {
    if (renderer == nullptr) return;
    //Net cheo trai
    thickLineRGBA(renderer, x, y + H, x + W / 2, y, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    //Net cheo phai
    thickLineRGBA(renderer, x + W, y + H, x + W / 2, y, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    //Net ngang
    thickLineRGBA(renderer, x + W/4, y + H/2, x + W - W/4, y + H/2, sdlOutlineThickness, color.r, color.g, color.b, color.a);
}
void thickArc(SDL_Renderer* renderer, int cx, int cy, int radius, int start_angle, int end_angle, int thickness, SDL_Color color) {
	//Ve nhieu cho net cong dc dam hon
    for (int i = 0; i < thickness; i++) {
        arcRGBA(renderer, cx, cy, radius - i, start_angle, end_angle, color.r, color.g, color.b, color.a);
    }
}
void drawHollowB(SDL_Renderer* renderer, int x, int y, int W, int H, int T, SDL_Color color, int sdlOutlineThickness) {
    if (renderer == nullptr) return;

    //Net doc
    thickLineRGBA(renderer, x, y, x, y + H, sdlOutlineThickness, color.r, color.g, color.b, color.a);

    // Tam va ban kinh 2 duong tron
    int radius = H / 4; 
    int cx = x + W / 2;
    int top_cy = y + radius;        
    int bottom_cy = y + H - radius;

    // Cung tron tren
    thickArc(renderer, cx, top_cy, radius, 270, 90, sdlOutlineThickness, color);
    // 2 duong noi cung tron voi net doc cho cung tron tren
    thickLineRGBA(renderer, x, y, x + W / 2, y, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    thickLineRGBA(renderer, x, y + H / 2, x + W / 2, y + H / 2, sdlOutlineThickness, color.r, color.g, color.b, color.a);

    // Cung tron duoi
    thickArc(renderer, cx, bottom_cy, radius, 270, 90, sdlOutlineThickness, color);
    // 2 duong noi cung tron voi net doc cho cung tron duoi
    thickLineRGBA(renderer, x, y + H / 2, x + W / 2, y + H / 2, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    thickLineRGBA(renderer, x, y + H, x + W / 2, y + H, sdlOutlineThickness, color.r, color.g, color.b, color.a);
}

void drawHollowC(SDL_Renderer* renderer, int x, int y, int W, int H, int T, SDL_Color color, int sdlOutlineThickness) {
    if (renderer == nullptr) return;
	W *= 3, H *= 3;
    // Tam va ban kinh
    int cx = x + W / 6;
    int cy = y + H / 6;
    int radius = (H < W ? H : W) / 2 - sdlOutlineThickness; 

    // Cung tron
    thickArc(renderer, cx, cy, radius, 45, 315, sdlOutlineThickness, color);
}

void drawHollowD(SDL_Renderer* renderer, int x, int y, int W, int H, int T, SDL_Color color, int sdlOutlineThickness) {
    if (renderer == nullptr) return;

    int leftX = x;
    int topY = y;
    int bottomY = y + H;

    //Net doc
    thickLineRGBA(renderer, leftX, topY, leftX, bottomY, sdlOutlineThickness, color.r, color.g, color.b, color.a);

    // Tam cung tron va ban kinh
    int cx = x + W / 2;
    int cy = y + H / 2;
    int radius = H / 2;

    // Cungt ron
    thickArc(renderer, cx, cy, radius, 270, 90, sdlOutlineThickness, color);

    // Net noi ngang o dau tren
    thickLineRGBA(renderer, leftX, topY, cx + radius * cos(270 * M_PI / 180.0), cy + radius * sin(270 * M_PI / 180.0),
                  sdlOutlineThickness, color.r, color.g, color.b, color.a);
    // Net noi ngang o dau duoi
    thickLineRGBA(renderer, leftX, bottomY, cx + radius * cos(90 * M_PI / 180.0), cy + radius * sin(90 * M_PI / 180.0),
                  sdlOutlineThickness, color.r, color.g, color.b, color.a);
}

void drawHollowE(SDL_Renderer* renderer, int x, int y, int W, int H, int T, SDL_Color color, int sdlOutlineThickness) {
    if (renderer == nullptr) return;
    //Net doc
    thickLineRGBA(renderer, x, y, x, y + H, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    //3 net ngang tu tren xuong duoi
    thickLineRGBA(renderer, x, y, x + H/2, y, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    thickLineRGBA(renderer, x, y + H/2, x + H/2, y + H/2, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    thickLineRGBA(renderer, x, y + H, x + H/2, y + H, sdlOutlineThickness, color.r, color.g, color.b, color.a);
}

void drawHollowF(SDL_Renderer* renderer, int x, int y, int W, int H, int T, SDL_Color color, int sdlOutlineThickness) {
    if (renderer == nullptr) return;
    // Net doc
    thickLineRGBA(renderer, x, y, x, y + H, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    //Net ngang tren
    thickLineRGBA(renderer, x, y, x + H/2, y, sdlOutlineThickness, color.r, color.g, color.b, color.a); 
    //Net ngang duoi
    thickLineRGBA(renderer, x, y + H/2, x + H/2, y + H/2, sdlOutlineThickness, color.r, color.g, color.b, color.a);
}


void drawHollowG(SDL_Renderer* renderer, int x, int y, int W, int H, int T, SDL_Color color, int sdlOutlineThickness) {
    if (renderer == nullptr) return;
	W *= 3, H *= 3;
    //Tam va ban kinh cung tron
    int cx = x + W / 6;
    int cy = y + H / 6;
    int radius = (H < W ? H : W) / 2 - sdlOutlineThickness;

    // Cung tron
    thickArc(renderer, cx, cy, radius, 45, 315, sdlOutlineThickness, color);

	//Toa do duong net doc
	int offsetX = 20;
	int offset = 103;
    int gx1 = cx + radius * cos(300 * M_PI / 180.0) + offsetX;
    int gy1 = cy + radius * sin(300 * M_PI / 180.0) + offset;
    int gx2 = cx + radius / 2 + offsetX;
    int gy2 = cy + offset - 30;
	
	//Net doc
    thickLineRGBA(renderer, gx1, gy1, gx2, gy2, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    //Net ngang
    thickLineRGBA(renderer, gx1 - 30, gy1, gx1, gy1, sdlOutlineThickness, color.r, color.g, color.b, color.a);
}

void drawHollowH(SDL_Renderer* renderer, int x, int y, int W, int H, int T, SDL_Color color, int sdlOutlineThickness) {
    if (renderer == nullptr) return;
    //Net doc trai
    thickLineRGBA(renderer, x, y, x, y + H, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    //Net ngang
    thickLineRGBA(renderer, x + H / 2, y, x + H / 2, y + H, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    //Net doc phai
    thickLineRGBA(renderer, x, y + H / 2, x + H/2, y + H/2, sdlOutlineThickness, color.r, color.g, color.b, color.a);
}
void drawHollowI(SDL_Renderer* renderer, int x, int y, int W, int H, int T, SDL_Color color, int sdlOutlineThickness) {
    if (renderer == nullptr) return;
    //Net doc
    thickLineRGBA(renderer, x + H / 4, y, x + H / 4, y + H, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    //Net ngang tren
    thickLineRGBA(renderer, x, y, x + H / 2, y, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    //Net ngang duoi
    thickLineRGBA(renderer, x, y + H, x + H / 2, y + H, sdlOutlineThickness, color.r, color.g, color.b, color.a);
}
void drawHollowJ(SDL_Renderer* renderer, int x, int y, int W, int H, int T, SDL_Color color, int sdlOutlineThickness) {
    if (renderer == nullptr) return;

    int topY = y;
    int bottomY = y + H;
    int rightX = x + W;

    // Net doc
    thickLineRGBA(renderer, rightX, topY, rightX, bottomY - H / 4, sdlOutlineThickness, color.r, color.g, color.b, color.a);

    // Tam va ban kinh cung tron
    int radius = W / 2;
    int cx = x + radius;
    int cy = y + H * 3 / 4 - radius + 30;

    // Net cong phia duoi chu J
    thickArc(renderer, cx, cy, radius, 0, 180, sdlOutlineThickness, color);

    // Net ngang
    thickLineRGBA(renderer, x, topY, rightX, topY, sdlOutlineThickness, color.r, color.g, color.b, color.a);
}

void drawHollowK(SDL_Renderer* renderer, int x, int y, int W, int H, int T, SDL_Color color, int sdlOutlineThickness) {
    if (renderer == nullptr) return;

    int leftX = x;
    int rightX = x + W;
    int topY = y;
    int bottomY = y + H;
    int midY = y + H / 2;

    // Net doc
    thickLineRGBA(renderer, leftX, topY, leftX, bottomY, sdlOutlineThickness, color.r, color.g, color.b, color.a);

    // Net cheo tren
    thickLineRGBA(renderer, leftX, midY, rightX, topY, sdlOutlineThickness, color.r, color.g, color.b, color.a);

    // Net cheo duoi
    thickLineRGBA(renderer, leftX, midY, rightX, bottomY, sdlOutlineThickness, color.r, color.g, color.b, color.a);
}

void drawHollowL(SDL_Renderer* renderer, int x, int y, int W, int H, int T, SDL_Color color, int sdlOutlineThickness) {
    if (renderer == nullptr) return;
    // Net doc
    thickLineRGBA(renderer, x, y, x, y + H, sdlOutlineThickness, color.r, color.g, color.b, color.a);
	//Net ngang
    thickLineRGBA(renderer, x, y + H, x + H/2, y + H, sdlOutlineThickness, color.r, color.g, color.b, color.a);
}

void drawHollowM(SDL_Renderer* renderer, int x, int y, int W, int H, int T, SDL_Color color, int sdlOutlineThickness) {
    if (renderer == nullptr) return;
    // Net doc trai
    thickLineRGBA(renderer, x, y, x, y + H, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    // Net cheo trai
    thickLineRGBA(renderer, x, y, x + W / 2, y + H / 2, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    //Net cheo phai
    thickLineRGBA(renderer, x + W / 2, y + H / 2, x + W, y, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    //Net doc phai
    thickLineRGBA(renderer, x + W, y, x + W, y + H, sdlOutlineThickness, color.r, color.g, color.b, color.a);
}

void renderPixelCanvas(SDL_Renderer* renderer) {
    if (renderer == nullptr) return;
    for (int r = 0; r < CANVAS_HEIGHT_CELLS; ++r) {
        for (int c = 0; c < CANVAS_WIDTH_CELLS; ++c) {
            SDL_Rect pixelRect = {
                PIXEL_ART_OFFSET_X + c * GRID_CELL_SIZE,
                PIXEL_ART_OFFSET_Y + r * GRID_CELL_SIZE,
                GRID_CELL_SIZE,
                GRID_CELL_SIZE
            };
            SDL_SetRenderDrawColor(renderer, pixelCanvas[r][c].r, pixelCanvas[r][c].g, pixelCanvas[r][c].b, pixelCanvas[r][c].a);
            SDL_RenderFillRect(renderer, &pixelRect);
        }
    }
}

void handlePixelCanvasInput(bool mousePressed) {
    if (mousePressed) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        if (mouseX >= PIXEL_ART_OFFSET_X && mouseX < PIXEL_ART_OFFSET_X + CANVAS_SCREEN_WIDTH &&
            mouseY >= PIXEL_ART_OFFSET_Y && mouseY < PIXEL_ART_OFFSET_Y + CANVAS_SCREEN_HEIGHT) {
            int canvasCellX = (mouseX - PIXEL_ART_OFFSET_X) / GRID_CELL_SIZE;
            int canvasCellY = (mouseY - PIXEL_ART_OFFSET_Y) / GRID_CELL_SIZE;
            if (canvasCellX >= 0 && canvasCellX < CANVAS_WIDTH_CELLS &&
                canvasCellY >= 0 && canvasCellY < CANVAS_HEIGHT_CELLS) {
                pixelCanvas[canvasCellY][canvasCellX] = currentDrawingColor;
            }
        }
    }
}

int main(int argc, char* args[]) {
    if (!initSDL()) {
        std::cerr << "Failed to initialize SDL!" << std::endl;
        return -1;
    }
    bool quit = false;
    SDL_Event e;
    bool leftMouseButtonDown = false;
    const int N_SDL_LINE_THICKNESS = 2;
    const int N_FRAME_MATERIAL_THICKNESS_IN_CELLS = 2;
    const int N_LANE_WIDTH_IN_CELLS = 3;
    const int N_TOTAL_HEIGHT_IN_CELLS = 26;
    int n_material_thickness_px = N_FRAME_MATERIAL_THICKNESS_IN_CELLS * GRID_CELL_SIZE;
    int n_outer_height_px = N_TOTAL_HEIGHT_IN_CELLS * GRID_CELL_SIZE;
    int diagonal_lane_width_px = N_LANE_WIDTH_IN_CELLS * GRID_CELL_SIZE;
    int n_outer_width_px = diagonal_lane_width_px + (2 * n_material_thickness_px);
    int n_pos_x_px = ((SCREEN_WIDTH - n_outer_width_px) / 2 / GRID_CELL_SIZE) * GRID_CELL_SIZE;
    int n_pos_y_px = ((SCREEN_HEIGHT - n_outer_height_px) / 2 / GRID_CELL_SIZE) * GRID_CELL_SIZE;
    SDL_Color n_frame_color = { 0, 0, 0, 255 };
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    leftMouseButtonDown = true;
                    handlePixelCanvasInput(leftMouseButtonDown);
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    leftMouseButtonDown = false;
                }
            }
            else if (e.type == SDL_MOUSEMOTION) {
                if (leftMouseButtonDown) {
                    handlePixelCanvasInput(leftMouseButtonDown);
                }
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_1:
                    currentDrawingColor = { 0, 0, 0, 255 };
                    break;
                case SDLK_2:
                    currentDrawingColor = { 255, 0, 0, 255 };
                    break;
                case SDLK_3:
                    currentDrawingColor = { 0, 255, 0, 255 };
                    break;
                case SDLK_4:
                    currentDrawingColor = { 0, 0, 255, 255 };
                    break;
                case SDLK_5:
                    currentDrawingColor = canvasBackgroundColor;
                    break;
                case SDLK_c:
                    pixelCanvas.assign(CANVAS_HEIGHT_CELLS, std::vector<SDL_Color>(CANVAS_WIDTH_CELLS, canvasBackgroundColor));
                    break;
                }
            }
        }
        renderPixelCanvas(gRenderer);
        drawHollowB(gRenderer, n_pos_x_px, n_pos_y_px, n_outer_width_px, n_outer_height_px, n_material_thickness_px, n_frame_color, N_SDL_LINE_THICKNESS);
        SDL_RenderPresent(gRenderer);
    }
    closeSDL();
    return 0;
}
