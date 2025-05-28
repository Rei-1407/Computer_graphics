#pragma once
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath> 

void drawHollowR(SDL_Renderer* renderer, int x, int y, int W, int H, SDL_Color color, int thickness) {
	if (renderer == nullptr) return;
	Sint16 x1 = x, x2 = x1 + W, x3 = x2 + 2 * W, x4 = x3 + W, x5 = x3 - W;
	Sint16 y1 = y, y2 = y1 + H / 8, y3 = y2 + H / 4, y4 = y3 + H / 8, y5 = y1 + H;
	int num_points = 4;
	int steps = 100;
	Sint16 x1_points[] = { x2, x4 + 10, x4 + 10, x2 };
	Sint16 y1_points[] = { y1, y1, y4, y4 };
	Sint16 x2_points[] = { x2, x3 + 10, x3 + 10, x2 };
	Sint16 y2_points[] = { y2, y2, y3, y3 };
	bezierRGBA(renderer, x1_points, y1_points, num_points, steps, color.r, color.g, color.b, color.a);
	bezierRGBA(renderer, x2_points, y2_points, num_points, steps, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x1, y, x2, y, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x1, y5, x2, y5, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x1, y, x1, y5, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x2, y, x2, y5, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x2, y4, x3, y5, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x5, y4 - 7, x4, y5, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x3, y5, x4, y5, thickness, color.r, color.g, color.b, color.a);
}