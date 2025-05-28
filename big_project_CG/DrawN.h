#pragma once
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath> 

void drawHollowN(SDL_Renderer* renderer, int x, int y, int W, int H, int T, SDL_Color color, int sdlOutlineThickness) {
    if (renderer == nullptr) return;
    thickLineRGBA(renderer, x, y, x, y + H, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    thickLineRGBA(renderer, x + T, y, x + T, y + H, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    thickLineRGBA(renderer, x + W, y, x + W, y + H, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    thickLineRGBA(renderer, x + W - T, y, x + W - T, y + H, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    thickLineRGBA(renderer, x, y, x + T, y, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    thickLineRGBA(renderer, x, y + H, x + T, y + H, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    thickLineRGBA(renderer, x + W, y, x + W - T, y, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    thickLineRGBA(renderer, x + W, y + H, x + W - T, y + H, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    thickLineRGBA(renderer, x, y, x + W - T, y + H, sdlOutlineThickness, color.r, color.g, color.b, color.a);
    thickLineRGBA(renderer, x + T, y, x + W, y + H, sdlOutlineThickness, color.r, color.g, color.b, color.a);
}