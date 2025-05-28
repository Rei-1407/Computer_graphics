#pragma once
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath> 

void drawHollowD(SDL_Renderer* renderer, int x, int y, int W, int H, SDL_Color color, int thickness) {
	if (renderer == nullptr) return;
	thickLineRGBA(renderer, x, y, x, y + H, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x + W / 2, y, x + W / 2, y + H, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x, y, x + W / 2, y, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x, y + H, x + W / 2, y + H, thickness, color.r, color.g, color.b, color.a);
	int num_points = 4;
	int steps = 100;
	Sint16 x1_points[] = { x, x + W * 5, x + W * 5, x };
	Sint16 y1_points[] = { y, y, y + H, y + H };
	bezierRGBA(renderer, x1_points, y1_points, num_points, steps, color.r, color.g, color.b, color.a);
	Sint16 x2_points[] = { x, x + 4 * W, x + 4 * W, x };
	Sint16 y2_points[] = { y + H / 10, y + H / 10, y + H * 9 / 10, y + H * 9 / 10 };
	bezierRGBA(renderer, x2_points, y2_points, num_points, steps, color.r, color.g, color.b, color.a);
}