#pragma once
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath> 

void drawHollowO(SDL_Renderer* renderer, int x, int y, int W, int H, int T, SDL_Color color) {
	if (renderer == nullptr || T <= 0 || W <= 0 || H <= 0) return;
	//center calculator
	int centerX = x + W / 2;
	int centerY = y + H / 2;
	//outer calculator
	int rx_outer = W / 2;
	int ry_outer = H / 2;
	//too lazy to explain this :(
	ellipseRGBA(renderer, centerX, centerY, rx_outer - 1, ry_outer - 1,
		color.r, color.g, color.b, color.a);
	ellipseRGBA(renderer, centerX, centerY, rx_outer - T + 1, ry_outer - T + 1,
		color.r, color.g, color.b, color.a);
}