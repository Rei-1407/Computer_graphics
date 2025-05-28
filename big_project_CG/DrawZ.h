#pragma once
#pragma once
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath> 

void drawHollowZ(SDL_Renderer* renderer, int x, int y, int W, int H, SDL_Color color, int thickness) {
	if (renderer == nullptr) return;
	Sint16 x1 = x, x2 = x1 + 4 * W, x4 = x2 - W, x3 = x1 + W + W/2;
	Sint16 y1 = y, y2 = y1 + H / 9, y3 = y + H - H / 9, y4 = y + H;
	thickLineRGBA(renderer, x1, y1, x2, y1, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x1, y2, x4, y2, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x1, y1, x1, y2, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x2, y1, x2, y2, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x4, y2, x1, y4, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x1, y4, x2, y4, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x2, y2, x3, y3, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x2, y3, x2, y4, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x3, y3, x2, y3, thickness, color.r, color.g, color.b, color.a);
}