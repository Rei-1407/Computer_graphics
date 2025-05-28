#pragma once
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath> 
//size screen
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
//canvas size
const int GRID_CELL_SIZE = 5;
const int PIXEL_ART_OFFSET_X = 0;
const int PIXEL_ART_OFFSET_Y = 0;
const int CANVAS_WIDTH_CELLS = SCREEN_WIDTH / GRID_CELL_SIZE;
const int CANVAS_HEIGHT_CELLS = SCREEN_HEIGHT / GRID_CELL_SIZE;
const int CANVAS_SCREEN_WIDTH = CANVAS_WIDTH_CELLS * GRID_CELL_SIZE;
const int CANVAS_SCREEN_HEIGHT = CANVAS_HEIGHT_CELLS * GRID_CELL_SIZE;
//drawing area
const int DRAWING_ALLOWED_AREA_X_START = 120;
const int DRAWING_ALLOWED_AREA_Y_START = 100;
const int DRAWING_ALLOWED_AREA_X_END = SCREEN_WIDTH - 120;
const int DRAWING_ALLOWED_AREA_Y_END = SCREEN_HEIGHT - 120;

//color
std::vector<std::vector<SDL_Color>> pixelCanvas;
SDL_Color currentDrawingColor = { 0, 0, 0, 255 };
SDL_Color canvasBackgroundColor = { 220, 220, 220, 255 };
SDL_Color DRAWING_AREA_BACKGROUND_COLOR = { 255, 255, 255, 255 };
SDL_Color blackColor = { 0, 0, 0, 255 };
// UI
enum ButtonState {
    BUTTON_NORMAL,
    BUTTON_HOVERED,
    BUTTON_CLICKED
};

// Audio
SDL_AudioDeviceID audioDevice = 0;
Uint8* bgmBuffer = nullptr;
Uint32 bgmLength = 0;
SDL_AudioSpec bgmSpec;
bool isMusicPaused = false;

Uint8* sfxBuffer = nullptr;
Uint32 sfxLength = 0;
SDL_AudioSpec sfxSpec;

//clear canvas after changed character
void initializePixelCanvasBackground() {
    pixelCanvas.resize(CANVAS_HEIGHT_CELLS, std::vector<SDL_Color>(CANVAS_WIDTH_CELLS));
    for (int r = 0; r < CANVAS_HEIGHT_CELLS; r++) {
        for (int c = 0; c < CANVAS_WIDTH_CELLS; c++) {
            int cell_pixel_x = PIXEL_ART_OFFSET_X + c * GRID_CELL_SIZE;
            int cell_pixel_y = PIXEL_ART_OFFSET_Y + r * GRID_CELL_SIZE;
            if (cell_pixel_x >= DRAWING_ALLOWED_AREA_X_START && cell_pixel_x < DRAWING_ALLOWED_AREA_X_END &&
                cell_pixel_y >= DRAWING_ALLOWED_AREA_Y_START && cell_pixel_y < DRAWING_ALLOWED_AREA_Y_END) {
                pixelCanvas[r][c] = DRAWING_AREA_BACKGROUND_COLOR;
            }
            else {
                pixelCanvas[r][c] = canvasBackgroundColor;
            }
        }
    }
}

//SDL initialize and close
bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }

    gWindow = SDL_CreateWindow("SDL Letter Drawing Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (gRenderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(gWindow);
        gWindow = nullptr;
        return false;
    }

    initializePixelCanvasBackground();
    initializeBitmapFontData(); // Initialize font data
    return true;
}

void closeSDL() {
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;
    gRenderer = nullptr;
    TTF_Quit();
    SDL_Quit();
}

//Audio setup
bool initBGM(const char* filename) {
    if (SDL_LoadWAV(filename, &bgmSpec, &bgmBuffer, &bgmLength) == NULL) {
        SDL_Log("Failed to load wav: %s", SDL_GetError()); return false;
    }
    bgmSpec.callback = NULL;
    audioDevice = SDL_OpenAudioDevice(NULL, 0, &bgmSpec, NULL, 0);
    if (!audioDevice) {
        SDL_Log("Failed to open audio: %s", SDL_GetError()); SDL_FreeWAV(bgmBuffer); bgmBuffer = nullptr; return false;
    }
    SDL_QueueAudio(audioDevice, bgmBuffer, bgmLength);
    SDL_PauseAudioDevice(audioDevice, 0);
    return true;
}
bool initSFX(const char* filename) {
    if (SDL_LoadWAV(filename, &sfxSpec, &sfxBuffer, &sfxLength) == NULL) {
        SDL_Log("Failed to load sound effect: %s", SDL_GetError()); return false;
    }
    return true;
}
void playSFX() {
    if (!sfxBuffer || sfxLength == 0) return;
    SDL_AudioDeviceID sfxDevice = SDL_OpenAudioDevice(NULL, 0, &sfxSpec, NULL, 0);
    if (!sfxDevice) { SDL_Log("Failed to open audio device for SFX: %s", SDL_GetError()); return; }
    SDL_ClearQueuedAudio(sfxDevice);
    SDL_QueueAudio(sfxDevice, sfxBuffer, sfxLength);
    SDL_PauseAudioDevice(sfxDevice, 0);
}
void playOrLoopBGM() {
    if (!audioDevice || !bgmBuffer || bgmLength == 0) return;
    Uint32 queued = SDL_GetQueuedAudioSize(audioDevice);
    if (!isMusicPaused && queued < bgmLength / 4) {
        SDL_QueueAudio(audioDevice, bgmBuffer, bgmLength);
    }
    if (isMusicPaused && SDL_GetAudioDeviceStatus(audioDevice) == SDL_AUDIO_PLAYING) { SDL_PauseAudioDevice(audioDevice, 1); }
    else if (!isMusicPaused && SDL_GetAudioDeviceStatus(audioDevice) == SDL_AUDIO_PAUSED) { SDL_PauseAudioDevice(audioDevice, 0); }
}
void cleanupBGM() {
    if (audioDevice) { SDL_PauseAudioDevice(audioDevice, 1); SDL_ClearQueuedAudio(audioDevice); SDL_CloseAudioDevice(audioDevice); audioDevice = 0; }
    if (bgmBuffer) { SDL_FreeWAV(bgmBuffer); bgmBuffer = nullptr; bgmLength = 0; }
}
void cleanupSFX() {
    if (sfxBuffer) { SDL_FreeWAV(sfxBuffer); sfxBuffer = nullptr; sfxLength = 0; }
}

//UI button and text
void drawButton(SDL_Renderer* renderer, int x, int y, int w, int h,
    const std::string& text, ButtonState state, SDL_Color textColor) {
    SDL_Color btnColor;
    switch (state) {
    case BUTTON_HOVERED: btnColor = { 210, 210, 210, 255 }; break;
    case BUTTON_CLICKED: btnColor = { 190, 190, 190, 255 }; break;
    default: btnColor = { 230, 230, 230, 255 }; break;
    }
    roundedBoxRGBA(renderer, x, y, x + w, y + h, 8, btnColor.r, btnColor.g, btnColor.b, btnColor.a);
    if (state == BUTTON_HOVERED || state == BUTTON_CLICKED) {
        roundedRectangleRGBA(renderer, x, y, x + w, y + h, 8, 100, 100, 100, 255);
    }
    else {
        roundedRectangleRGBA(renderer, x, y, x + w, y + h, 8, 150, 150, 150, 255);
    }
    int textRenderWidth = (int)text.length() * 8;
    int textRenderHeight = 8;
    int textX = x + (w - textRenderWidth) / 2;
    int textY = y + (h - textRenderHeight) / 2;
    stringRGBA(renderer, textX, textY, text.c_str(), textColor.r, textColor.g, textColor.b, textColor.a);
}

void drawText(SDL_Renderer* renderer, const std::string& text, int x, int y, SDL_Color color) {
    stringRGBA(renderer, x, y, text.c_str(), color.r, color.g, color.b, color.a);
}

//render pixel canvas
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

//handle pixel canvas input
void handlePixelCanvasInput(bool mousePressed) {
    if (mousePressed) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        if (mouseX >= DRAWING_ALLOWED_AREA_X_START && mouseX < DRAWING_ALLOWED_AREA_X_END
            && mouseY >= DRAWING_ALLOWED_AREA_Y_START && mouseY < DRAWING_ALLOWED_AREA_Y_END) {
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
}