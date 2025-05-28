#pragma once
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath> 

void drawHollowT(SDL_Renderer* renderer, int x, int y, int W, int H, SDL_Color color, int thickness) {
	if (renderer == nullptr) return;
	int h = H / 9;
	thickLineRGBA(renderer, x - W/2, y, x - W/2, y + H, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x - W * 4, y, x + W * 4, y, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x + W * 4, y, x + W * 4, y + h, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x - W * 4, y, x - W * 4, y + h, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x - W * 4, y + h, x + W * 4, y + h, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x + W / 2, y, x + W / 2, y + H, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x, y, x + W / 2, y, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x - W/2, y + H, x + W / 2, y + H, thickness, color.r, color.g, color.b, color.a);
}