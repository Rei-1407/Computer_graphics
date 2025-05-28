#pragma once
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath> 

void drawHollowX(SDL_Renderer* renderer, int x, int y, int W, int H, SDL_Color color, int thickness) {
	if (renderer == nullptr) return;
	Sint16 x3 = static_cast<Sint16> (x) + W * 4, x2 = static_cast<Sint16> (x) + W/2, x4 = x3 + W/2;
	thickLineRGBA(renderer, x, y, x3, y + H, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x3, y, x, y + H, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x2, y, x4, y + H, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x4, y, x2, y + H, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x, y, x2, y, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x3, y, x4, y, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x, y + H, x2, y + H, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x3, y + H, x4, y + H, thickness, color.r, color.g, color.b, color.a);
}