#pragma once
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath> 

void drawHollowV(SDL_Renderer* renderer, int x, int y, int W, int H, SDL_Color color, int thickness) {
	if (renderer == nullptr) return;
	Sint16 x1 = x, x2 = x1 + W / 2, x3 = x2 + W / 2, x4 = x3 + W / 2, x5 = x4 + W, x6 = x5 + W / 2, x7 = x6 + W / 2, x8 = x7 + W / 2;
	Sint16 y1 = y, y2 = y1 + H * 9 / 10, y3 = y1 + H;
	thickLineRGBA(renderer, x1, y1, x2, y1, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x7, y1, x8, y1, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x4, y2, x5, y2, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x3, y3, x6, y3, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x1, y1, x3, y3, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x2, y1, x4, y2, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x5, y2, x7, y1, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x6, y3, x8, y1, thickness, color.r, color.g, color.b, color.a);
}