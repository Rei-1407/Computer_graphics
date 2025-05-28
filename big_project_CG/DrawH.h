#pragma once
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath> 

void drawHollowH(SDL_Renderer* renderer, int x, int y, int W, int H, SDL_Color color, int thickness) {
	if (renderer == nullptr) return;
	thickLineRGBA(renderer, x - W*2, y, x - W * 2, y + H, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x - 3 * W / 2, y, x - 3 * W / 2, y + H, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x - 3 * W / 2, y, x -  W * 2, y, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x - W * 2, y + H, x - 3 * W / 2, y + H, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x + W * 2, y, x + W * 2, y + H, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x + W / 2 + W * 2, y, x + W / 2 + W * 2, y + H, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x + W * 2, y, x + W / 2 + W * 2, y, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x + W * 2, y + H, x + W / 2 + W * 2, y + H, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x - 3 * W / 2, y + H / 2 - H / 10 + W, x + W * 2, y + H / 2 - H / 10 + W, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x - 3 * W / 2, y + H / 2 + H / 10 - W, x + W * 2, y + H / 2 + H / 10 - W, thickness, color.r, color.g, color.b, color.a);
}