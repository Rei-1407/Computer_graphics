#pragma once
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath> 

void drawHollowK(SDL_Renderer* renderer, int x, int y, int W, int H, SDL_Color color, int thickness) {
	if (renderer == nullptr) return;
	Sint16 x2 = x + W / 2, x3 = x + W * 4, x4 = x3 + W / 2, x5 = x2 + W/2;
	Sint16 y2 = y + H * 2 / 5, y3 = y + H / 2, y4 = y + H / 2, y5 = y + H;
	thickLineRGBA(renderer, x, y, x, y5, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x2, y, x2, y5, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x, y, x2, y, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x, y5, x2, y5, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x3, y, x2, y2, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x3, y, x4, y, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x3, y5, x4, y5, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x4, y, x2, y4, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x5, y4 - W/4 - W/6, x4, y5, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x2, y4, x3, y5, thickness, color.r, color.g, color.b, color.a);
}