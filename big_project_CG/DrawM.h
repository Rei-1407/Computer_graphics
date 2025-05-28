#pragma once
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath> 

void drawHollowM(SDL_Renderer* renderer, int x, int y, int W, int H, SDL_Color color, int thickness) {
	if (renderer == nullptr) return;
	Sint16 x1 = x, x2 = x1 + W, x3 = x2 + 2 * W, x4 = x3 + 2 * W, x5 = x4 + W;
	Sint16 y1 = y, y2 = y1 + H / 8, y3 = y2 + H / 4, y4 = y3 + H / 8, y5 = y1 + H;
	thickLineRGBA(renderer, x1, y1, x2, y1, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x1, y5, x2, y5, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x4, y1, x5, y1, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x4, y5, x5, y5, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x2, y1, x3, y3, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x4, y1, x3, y3, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x2, y2, x3, y4, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x4, y2, x3, y4, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x1, y1, x1, y5, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x2, y2, x2, y5, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x4, y2, x4, y5, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x5, y1, x5, y5, thickness, color.r, color.g, color.b, color.a);
}