#pragma once
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath> 

void drawHollowW(SDL_Renderer* renderer, int x, int y, int W, int H, SDL_Color color, int thickness) {
	if (renderer == nullptr) return;
	Sint16 x1 = x, x2 = x1 + W / 2, x3 = x2 + W / 2, x4 = x3 + W / 2, x5 = x4 + W/2, x6 = x5 + W / 2, x7 = x6 + W / 2, x8 = x7 + W / 2;
	Sint16 y1 = y, y2 = y1 + H * 9 / 10, y3 = y1 + H;
	Sint16 X1 = x7, X2 = X1 + W / 2, X3 = X2 + W / 2, X4 = X3 + W / 2, X5 = X4 + W / 2, X6 = X5 + W / 2, X7 = X6 + W / 2, X8 = X7 + W / 2;
	thickLineRGBA(renderer, x1, y1, x2, y1, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x7, y1 + H/3, x8, y1 + H/3, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x4, y2, x5, y2, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x3, y3, x6, y3, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x1, y1, x3, y3, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x2, y1, x4, y2, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x5, y2, x7, y1 + H/3, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x6, y3, x8, y1 + H/3, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, X1, y1 + H/3, X2, y1 + H/3, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, X7, y1, X8, y1, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, X4, y2, X5, y2, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, X3, y3, X6, y3, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, X1, y1 + H/3, X3, y3, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, X2, y1 + H/3, X4, y2, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, X5, y2, X7, y1, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, X6, y3, X8, y1, thickness, color.r, color.g, color.b, color.a);
}