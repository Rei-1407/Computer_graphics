#pragma once
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath> 

std::vector<std::vector<std::vector<int>>> fontData;
const int BITMAP_CHAR_HEIGHT = 7;

void initializeBitmapFontData() {
    fontData.resize(26);
    fontData[0] = { {0,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,1,1,1,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1} }; // A
    fontData[1] = { {1,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,1,1,1,0} }; // B
    fontData[2] = { {0,1,1,1,0},{1,0,0,0,1},{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,1},{0,1,1,1,0} }; // C
    fontData[3] = { {1,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,1,1,1,0} }; // D
    fontData[4] = { {1,1,1,1,1},{1,0,0,0,0},{1,0,0,0,0},{1,1,1,1,0},{1,0,0,0,0},{1,0,0,0,0},{1,1,1,1,1} }; // E
    fontData[5] = { {1,1,1,1,1},{1,0,0,0,0},{1,0,0,0,0},{1,1,1,1,0},{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0} }; // F
    fontData[6] = { {0,1,1,1,0},{1,0,0,0,0},{1,0,0,0,0},{1,0,1,1,1},{1,0,0,0,1},{1,0,0,0,1},{0,1,1,1,1} }; // G
    fontData[7] = { {1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,1,1,1,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1} }; // H
    fontData[8] = { {1,1,1},{0,1,0},{0,1,0},{0,1,0},{0,1,0},{0,1,0},{1,1,1} }; // I
    fontData[9] = { {0,0,1,1,1},{0,0,0,1,0},{0,0,0,1,0},{0,0,0,1,0},{1,0,0,1,0},{1,0,0,1,0},{0,1,1,0,0} }; // J
    fontData[10] = { {1,0,0,1},{1,0,1,0},{1,1,0,0},{1,0,1,0},{1,0,1,0},{1,0,0,1},{1,0,0,1} }; // K
    fontData[11] = { {1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,1,1,1} }; // L
    fontData[12] = { {1,0,0,0,1},{1,1,0,1,1},{1,0,1,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1} }; // M
    fontData[13] = { {1,0,0,0,1},{1,1,0,0,1},{1,0,1,0,1},{1,0,0,1,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1} }; // N
    fontData[14] = { {0,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{0,1,1,1,0} }; // O
    fontData[15] = { {1,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,1,1,1,0},{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0} }; // P
    fontData[16] = { {0,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,0,1,0,1},{1,0,0,1,1},{0,1,1,0,1},{0,0,0,0,1} }; // Q
    fontData[17] = { {1,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,1,1,1,0},{1,0,1,0,0},{1,0,0,1,0},{1,0,0,0,1} }; // R
    fontData[18] = { {0,1,1,1,1},{1,0,0,0,0},{0,1,1,1,0},{0,0,0,0,1},{1,1,1,1,0} }; // S
    fontData[19] = { {1,1,1,1,1},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0} }; // T
    fontData[20] = { {1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{0,1,1,1,0} }; // U
    fontData[21] = { {1,0,0,0,1},{1,0,0,0,1},{0,1,0,1,0},{0,1,0,1,0},{0,0,1,0,0} }; // V
    fontData[22] = { {1,0,0,0,1},{1,0,1,0,1},{1,0,1,0,1},{1,0,1,0,1},{0,1,0,1,0} }; // W
    fontData[23] = { {1,0,0,0,1},{0,1,0,1,0},{0,0,1,0,0},{0,1,0,1,0},{1,0,0,0,1} }; // X
    fontData[24] = { {1,0,0,0,1},{0,1,0,1,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0} }; // Y
    fontData[25] = { {1,1,1,1,1},{0,0,0,1,0},{0,0,1,0,0},{0,1,0,0,0},{1,1,1,1,1} }; // Z
}

void drawBitmapCharacterPreview(SDL_Renderer* renderer, char c, int top_left_x, int top_left_y,
    int preview_box_width, int preview_box_height, SDL_Color color) {
    if (c < 'A' || c > 'Z') return;
    if (renderer == nullptr) return;

    int charIndex = c - 'A';
    if (charIndex >= fontData.size() || fontData[charIndex].empty()) return;

    const auto& mask = fontData[charIndex];
    int bitmap_H = mask.size();
    if (bitmap_H == 0) return;
    int bitmap_W = mask[0].size();
    if (bitmap_W == 0) return;

    float scaleX = (float)preview_box_width / bitmap_W;
    float scaleY = (float)preview_box_height / bitmap_H;
    int preview_pixel_size = static_cast<int>(std::min(scaleX, scaleY));
    if (preview_pixel_size < 1) preview_pixel_size = 1; 

    int actual_char_width = bitmap_W * preview_pixel_size;
    int actual_char_height = bitmap_H * preview_pixel_size;

    int char_offset_x = top_left_x + (preview_box_width - actual_char_width) / 2;
    int char_offset_y = top_left_y + (preview_box_height - actual_char_height) / 2;

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int r = 0; r < bitmap_H; ++r) {
        for (int col = 0; col < bitmap_W; ++col) {
            if (mask[r][col] != 0) {
                SDL_Rect pixelRect = {
                    char_offset_x + col * preview_pixel_size,
                    char_offset_y + r * preview_pixel_size,
                    preview_pixel_size,
                    preview_pixel_size
                };
                SDL_RenderFillRect(renderer, &pixelRect);
            }
        }
    }
}