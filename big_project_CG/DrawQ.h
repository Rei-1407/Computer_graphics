#pragma once
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

void drawHollowQ(SDL_Renderer* renderer, int x, int y, int W, int H, int T, SDL_Color color) {
    if (renderer == nullptr || T <= 0 || W <= 0 || H <= 0) return;

    int centerX = x + W / 2;
    int centerY = y + H / 2;
    int rx_outer = W / 2;
    int ry_outer = H / 2;
    ellipseRGBA(renderer, centerX, centerY, rx_outer, ry_outer, 
        color.r, color.g, color.b, color.a);
    if (rx_outer - T > 0 && ry_outer - T > 0) {
        ellipseRGBA(renderer, centerX, centerY, rx_outer - T, ry_outer - T,
            color.r, color.g, color.b, color.a);
    }
    else { 
        ellipseRGBA(renderer, centerX, centerY, 1, 1,
            color.r, color.g, color.b, color.a);
    }
    int tail_startX = centerX + rx_outer / 2; 
    int tail_startY = centerY + ry_outer / 2; 

    int tail_endX = centerX + rx_outer;     
    int tail_endY = centerY + ry_outer;    
    int tail_thickness = 2;
    thickLineRGBA(renderer, tail_startX + 13, tail_startY, tail_endX + 13, tail_endY,
        tail_thickness, color.r, color.g, color.b, color.a);
    thickLineRGBA(renderer, tail_startX + T * 2 - 12, tail_startY, tail_endX + T * 2 - 12, tail_endY,
        tail_thickness, color.r, color.g, color.b, color.a);
    thickLineRGBA(renderer, tail_endX + 13, tail_endY, tail_endX + T * 2 - 12, tail_endY,
        tail_thickness, color.r, color.g, color.b, color.a);
}