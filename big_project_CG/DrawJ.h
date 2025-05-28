#pragma once
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath> 

void drawHollowJ(SDL_Renderer* renderer, int x, int y, int W, int H, SDL_Color color, int thickness) {
	if (renderer == nullptr) return;
	Sint16 x1 = x, x2 = x1 + W, x3 = x2 + 2 * W, x4 = x3 + W;
	Sint16 y1 = y, y2 = y1 + 4 * H / 5 - H/8, y3 = y2 + H / 10, y4 = y1 + H;
	int num_points = 4;
	int steps = 100;
	Sint16 x1_points[] = { x1,x1,x4,x4 };
	Sint16 y1_points[] = { y2, y4, y4, y2 };
	Sint16 x2_points[] = { x2,x2,x3,x3 };
	Sint16 y2_points[] = { y2, y3, y3, y2 };
	bezierRGBA(renderer, x1_points, y1_points, num_points, steps, color.r, color.g, color.b, color.a);
	bezierRGBA(renderer, x2_points, y2_points, num_points, steps, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x1, y2 - H/8, x2, y2 - H/8, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x1, y2 - H / 8, x1, y2, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x2, y2 - H / 8, x2, y2, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x3, y1, x4, y1, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x3, y2, x3, y1, thickness, color.r, color.g, color.b, color.a);
	thickLineRGBA(renderer, x4, y2, x4, y1, thickness, color.r, color.g, color.b, color.a);
}