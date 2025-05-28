#pragma once
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath> 

void drawHollowS(SDL_Renderer* renderer, int x, int y, int W, int H, SDL_Color color, int thickness) {
	if (renderer == nullptr) return;
	Sint16 x1 = x, x2 = x1 + W / 2, x3 = x2 + 2 * W, x4 = x3 + W / 2;
	Sint16 y1 = y, y2 = y1 + H / 8, y3 = y2 + H / 4, y4 = y3 + H / 8, y5 = y1 + H - H/8, y6 = y1 + H;
	thickLineRGBA(renderer, x1, y, x4, y, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x2, y2, x4, y2, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x2, y3, x4, y3, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x1, y4, x3, y4, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x1, y5, x3, y5, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x1, y6, x4, y6, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x1, y1, x1, y4, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x2, y2, x2, y3, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x4, y1, x4, y2, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x3, y4, x3, y5, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x4, y3, x4, y6, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x1, y5, x1, y6, thickness, color.r, color.g, color.b, color.a);
}