#pragma once
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath> 

void drawHollowA(SDL_Renderer* renderer, int x, int y, int W, int H, SDL_Color color, int thickness) {
	if (renderer == nullptr) return;
	thickLineRGBA(renderer, x, y, x + W, y, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x, y, x - W * 2, y + H, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x - W * 2, y + H, x - W, y + H, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x - W, y + H, x, y + H / 2, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x + W, y, x + W * 3, y + H, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x + W * 3, y + H, x + 2 * W, y + H, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x + W * 2, y + H, x + W, y + H / 2, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x, y + H / 2, x + W, y + H / 2, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x, y + H * 13/30, x + W, y + H * 13/30, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x, y + H * 13/30, x + W / 2, y + H/4, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x + W, y + H * 13 / 30, x + W / 2, y + H / 4, thickness, color.r, color.g, color.b, color.a);
}