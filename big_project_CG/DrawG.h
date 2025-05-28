#pragma once
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath> 

void drawHollowG(SDL_Renderer* renderer, int x, int y, int W, int H, SDL_Color color, int thickness) {
	if (renderer == nullptr) return;
	int h = H / 9;
	thickLineRGBA(renderer, x - W/2, y, x - W/2, y + H, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x + W / 2, y, x + W / 2, y + H, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x - W/2, y, x + W / 2, y, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x, y, x + W * 3, y, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x + W * 3, y, x + W * 3, y + h, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x - W/2, y + h, x + W * 3, y + h, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x - W/2, y + h * 8, x + W * 3, y + h * 8, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x, y + H, x + W * 3, y + H, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x - W/2, y + H, x + W * 4, y + H, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x + W * 4, y + h * 5, x + W * 4, y + H, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x + W * 3, y + h * 8, x + W * 3, y + h * 6, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x + W * 2, y + h * 5, x + W * 4, y + h * 5, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x + W * 2, y + h * 5, x + W * 2, y + h * 6, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x + W * 2, y + h * 6, x + W * 3, y + h * 6, thickness, color.r, color.g, color.b, color.a);
}