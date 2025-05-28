#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath> 
#include "Bitmap.h"
#include "SDL_set_up.h"
#include "DrawO.h"
#include "DrawA.h"
#include "DrawB.h"
#include "DrawP.h"
#include "DrawN.h"
#include "DrawI.h"
#include "DrawC.h"
#include "DrawD.h"
#include "DrawE.h"
#include "DrawF.h"
#include "DrawG.h"
#include "DrawH.h"
#include "DrawL.h"
#include "DrawT.h"
#include "DrawX.h"
#include "DrawK.h"
#include "DrawZ.h"
#include "DrawV.h"
#include "DrawW.h"
#include "DrawY.h"
#include "DrawU.h"
#include "DrawJ.h"
#include "DrawM.h"
#include "DrawQ.h"
#include "DrawR.h"
#include "DrawS.h"

int main(int argc, char* argv[]) {
    if (!initSDL()) {
        return -1;
    }

    if (!initBGM("C:\\Users\\ghlhn\\Downloads\\DA\\Computer_graphics\\sounds\\bgm.wav")) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Audio Error", "Could not load sounds/bgm.wav", NULL);
    }
    if (!initSFX("C:\\Users\\ghlhn\\Downloads\\DA\\Computer_graphics\\sounds\\cursor.wav")) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Audio Error", "Could not load sounds/cursor.wav", NULL);
    }

    std::vector<std::string> chars_str_list;
    for (char c_val = 'A'; c_val <= 'Z'; ++c_val) {
        chars_str_list.push_back(std::string(1, c_val));
    }
    int currentCharIndex = 0;

    SDL_Color textColor = { 0, 0, 0, 255 };
    SDL_Color redColor = { 255, 0, 0, 255 };
    SDL_Color greenColor = { 0, 128, 0, 255 };

    int btnW = 120;
    int btnH = 50;
    int btnY = SCREEN_HEIGHT - btnH - 70;
    int prevBtnX = SCREEN_WIDTH / 2 - btnW - 10 - btnW / 2; // Adjusted button positions
    int nextBtnX = SCREEN_WIDTH / 2 + 10 + btnW / 2;


    const int pauseBtnW = 120;
    const int pauseBtnX = SCREEN_WIDTH / 2 - pauseBtnW / 2;
    const int pauseBtnY = SCREEN_HEIGHT - btnH - 20;
    ButtonState pauseBtnState = BUTTON_NORMAL;
    std::string pauseBtnText = "Pause BGM";


    ButtonState prevBtnState = BUTTON_NORMAL;
    ButtonState nextBtnState = BUTTON_NORMAL;
    bool ui_mouseDown = false;

    const int CHAR_SDL_OUTLINE_THICKNESS = 2;
    const int CHAR_FRAME_MATERIAL_THICKNESS_IN_CELLS = 3;
    const int CHAR_LANE_WIDTH_IN_CELLS = 4;
    const int CHAR_TOTAL_HEIGHT_IN_CELLS = 35;

    int char_material_thickness_px = CHAR_FRAME_MATERIAL_THICKNESS_IN_CELLS * GRID_CELL_SIZE * 2;
    int char_outer_height_px = CHAR_TOTAL_HEIGHT_IN_CELLS * GRID_CELL_SIZE * 2;
    SDL_Color char_frame_color = { 0, 0, 0, 255 };


    bool quit = false;
    SDL_Event e;
    bool pixel_leftMouseButtonDown = false;

    while (!quit) {
        playOrLoopBGM();

        int mx, my;
        Uint32 mouseState = SDL_GetMouseState(&mx, &my);

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    ui_mouseDown = true;

                    bool onAnyButton = (mx >= prevBtnX && mx <= prevBtnX + btnW && my >= btnY && my <= btnY + btnH) ||
                        (mx >= nextBtnX && mx <= nextBtnX + btnW && my >= btnY && my <= btnY + btnH) ||
                        (mx >= pauseBtnX && mx <= pauseBtnX + pauseBtnW && my >= pauseBtnY && my <= pauseBtnY + btnH);
                    if (!onAnyButton) {
                        pixel_leftMouseButtonDown = true;
                        handlePixelCanvasInput(pixel_leftMouseButtonDown);
                    }
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    bool prevHovered_up = (mx >= prevBtnX && mx <= prevBtnX + btnW && my >= btnY && my <= btnY + btnH);
                    bool nextHovered_up = (mx >= nextBtnX && mx <= nextBtnX + btnW && my >= btnY && my <= btnY + btnH);
                    bool pauseHovered_up = (mx >= pauseBtnX && mx <= pauseBtnX + pauseBtnW && my >= pauseBtnY && my <= pauseBtnY + btnH);

                    if (prevBtnState == BUTTON_CLICKED && prevHovered_up) {
                        if (currentCharIndex > 0) currentCharIndex--;
                        initializePixelCanvasBackground();
                        playSFX();
                    }
                    if (nextBtnState == BUTTON_CLICKED && nextHovered_up) {
                        if (currentCharIndex < (int)chars_str_list.size() - 1) currentCharIndex++;
                        initializePixelCanvasBackground();
                        playSFX();
                    }
                    if (pauseBtnState == BUTTON_CLICKED && pauseHovered_up) {
                        isMusicPaused = !isMusicPaused;
                        if (isMusicPaused) SDL_PauseAudioDevice(audioDevice, 1);
                        else SDL_PauseAudioDevice(audioDevice, 0);
                        playSFX();
                    }
                    ui_mouseDown = false;
                    pixel_leftMouseButtonDown = false;
                }
            }
            else if (e.type == SDL_MOUSEMOTION) {
                if (pixel_leftMouseButtonDown) {
                    bool onPrev = (mx >= prevBtnX && mx <= prevBtnX + btnW && my >= btnY && my <= btnY + btnH);
                    bool onNext = (mx >= nextBtnX && mx <= nextBtnX + btnW && my >= btnY && my <= btnY + btnH);
                    bool onPause = (mx >= pauseBtnX && mx <= pauseBtnX + pauseBtnW && my >= pauseBtnY && my <= pauseBtnY + btnH);
                    if (!onPrev && !onNext && !onPause) {
                        handlePixelCanvasInput(pixel_leftMouseButtonDown);
                    }
                }
            }
            else if (e.type == SDL_KEYDOWN) {
                int oldCharIndex = currentCharIndex;
                if (e.key.keysym.sym >= SDLK_a && e.key.keysym.sym <= SDLK_z) {
                    char pressedChar = toupper(e.key.keysym.sym);
                    for (size_t i = 0; i < chars_str_list.size(); ++i) {
                        if (chars_str_list[i][0] == pressedChar) {
                            currentCharIndex = (int)i;
                            break;
                        }
                    }
                }
                switch (e.key.keysym.sym) {
                case SDLK_F1: currentDrawingColor = { 0,0,0,255 }; break;
                case SDLK_F2: currentDrawingColor = { 52, 216,235,255 }; break;
                case SDLK_F3: currentDrawingColor = { 0,255,0,255 }; break;
                case SDLK_F4: currentDrawingColor = { 0,0,255,255 }; break;
                case SDLK_F5: currentDrawingColor = DRAWING_AREA_BACKGROUND_COLOR; break;
                case SDLK_F6:
                    initializePixelCanvasBackground();
                    break;
                case SDLK_LEFT:
                    if (currentCharIndex > 0) currentCharIndex--;
                    playSFX();
                    break;
                case SDLK_RIGHT:
                    if (currentCharIndex < (int)chars_str_list.size() - 1) currentCharIndex++;
                    playSFX();
                    break;
                }
                if (oldCharIndex != currentCharIndex) {
                    initializePixelCanvasBackground();
                }
            }
        }

        bool prevHovered = (mx >= prevBtnX && mx <= prevBtnX + btnW && my >= btnY && my <= btnY + btnH);
        bool nextHovered = (mx >= nextBtnX && mx <= nextBtnX + btnW && my >= btnY && my <= btnY + btnH);
        bool pauseHovered = (mx >= pauseBtnX && mx <= pauseBtnX + pauseBtnW && my >= pauseBtnY && my <= pauseBtnY + btnH);

        prevBtnState = prevHovered ? (ui_mouseDown ? BUTTON_CLICKED : BUTTON_HOVERED) : BUTTON_NORMAL;
        nextBtnState = nextHovered ? (ui_mouseDown ? BUTTON_CLICKED : BUTTON_HOVERED) : BUTTON_NORMAL;
        pauseBtnState = pauseHovered ? (ui_mouseDown ? BUTTON_CLICKED : BUTTON_HOVERED) : BUTTON_NORMAL;
        pauseBtnText = isMusicPaused ? "Resume BGM" : "Pause BGM";


        SDL_SetRenderDrawColor(gRenderer, canvasBackgroundColor.r, canvasBackgroundColor.g, canvasBackgroundColor.b, canvasBackgroundColor.a);
        SDL_RenderClear(gRenderer);

        renderPixelCanvas(gRenderer);

        char selectedChar = chars_str_list[currentCharIndex][0];
        int current_char_W = (CHAR_LANE_WIDTH_IN_CELLS + 2 * CHAR_FRAME_MATERIAL_THICKNESS_IN_CELLS) * GRID_CELL_SIZE;
        int char_pos_x_px = ((SCREEN_WIDTH - current_char_W) / 2 / GRID_CELL_SIZE) * GRID_CELL_SIZE;
        int char_pos_y_px = ((SCREEN_HEIGHT - char_outer_height_px) / 2 / GRID_CELL_SIZE) * GRID_CELL_SIZE;

        switch (selectedChar) {
        case 'A': drawHollowA(gRenderer, char_pos_x_px, char_pos_y_px, current_char_W, char_outer_height_px, char_frame_color, CHAR_SDL_OUTLINE_THICKNESS); break;
        case 'B': drawHollowB(gRenderer, char_pos_x_px, char_pos_y_px, current_char_W, char_outer_height_px, char_frame_color, CHAR_SDL_OUTLINE_THICKNESS); break;
        case 'C': drawHollowC(gRenderer, char_pos_x_px + 100, char_pos_y_px, current_char_W, char_outer_height_px, char_frame_color, CHAR_SDL_OUTLINE_THICKNESS); break;
        case 'D': drawHollowD(gRenderer, char_pos_x_px, char_pos_y_px, current_char_W, char_outer_height_px, char_frame_color, CHAR_SDL_OUTLINE_THICKNESS); break;
        case 'E': drawHollowE(gRenderer, char_pos_x_px - 50, char_pos_y_px, current_char_W, char_outer_height_px, char_frame_color, CHAR_SDL_OUTLINE_THICKNESS); break;
        case 'F': drawHollowF(gRenderer, char_pos_x_px, char_pos_y_px, current_char_W, char_outer_height_px, char_frame_color, CHAR_SDL_OUTLINE_THICKNESS); break;
        case 'G': drawHollowG(gRenderer, char_pos_x_px - 30, char_pos_y_px, current_char_W, char_outer_height_px, char_frame_color, CHAR_SDL_OUTLINE_THICKNESS); break;
        case 'H': drawHollowH(gRenderer, char_pos_x_px + 30, char_pos_y_px, current_char_W, char_outer_height_px, char_frame_color, CHAR_SDL_OUTLINE_THICKNESS); break;
        case 'I': drawHollowI(gRenderer, char_pos_x_px + 15, char_pos_y_px, current_char_W, char_outer_height_px, char_frame_color, CHAR_SDL_OUTLINE_THICKNESS); break;
        case 'J': drawHollowJ(gRenderer, char_pos_x_px - 75, char_pos_y_px, current_char_W, char_outer_height_px, char_frame_color, CHAR_SDL_OUTLINE_THICKNESS); break;
        case 'K': drawHollowK(gRenderer, char_pos_x_px - 50, char_pos_y_px, current_char_W, char_outer_height_px, char_frame_color, CHAR_SDL_OUTLINE_THICKNESS); break;
        case 'L': drawHollowL(gRenderer, char_pos_x_px, char_pos_y_px, current_char_W, char_outer_height_px, char_frame_color, CHAR_SDL_OUTLINE_THICKNESS); break;
        case 'M': drawHollowM(gRenderer, char_pos_x_px - 125, char_pos_y_px, current_char_W, char_outer_height_px, char_frame_color, CHAR_SDL_OUTLINE_THICKNESS); break;
        case 'N': drawHollowN(gRenderer, char_pos_x_px - 80, char_pos_y_px, current_char_W + 160, char_outer_height_px, char_material_thickness_px, char_frame_color, CHAR_SDL_OUTLINE_THICKNESS); break;
        case 'O': drawHollowO(gRenderer, char_pos_x_px - 100, char_pos_y_px, current_char_W + 200, char_outer_height_px, char_material_thickness_px, char_frame_color); break;
        case 'P': drawHollowP(gRenderer, char_pos_x_px, char_pos_y_px, current_char_W, char_outer_height_px, char_frame_color, CHAR_SDL_OUTLINE_THICKNESS); break;
        case 'Q': drawHollowQ(gRenderer, char_pos_x_px - 100, char_pos_y_px, current_char_W + 200, char_outer_height_px, char_material_thickness_px, char_frame_color); break;
        case 'R': drawHollowR(gRenderer, char_pos_x_px - 50, char_pos_y_px, current_char_W, char_outer_height_px, char_frame_color, CHAR_SDL_OUTLINE_THICKNESS); break;
        case 'S': drawHollowS(gRenderer, char_pos_x_px - 50, char_pos_y_px, current_char_W, char_outer_height_px, char_frame_color, CHAR_SDL_OUTLINE_THICKNESS); break;
        case 'T': drawHollowT(gRenderer, char_pos_x_px + 25, char_pos_y_px, current_char_W, char_outer_height_px, char_frame_color, CHAR_SDL_OUTLINE_THICKNESS); break;
        case 'U': drawHollowU(gRenderer, char_pos_x_px - 100, char_pos_y_px + 75, current_char_W, char_outer_height_px, char_frame_color, CHAR_SDL_OUTLINE_THICKNESS); break;
        case 'V': drawHollowV(gRenderer, char_pos_x_px - 70, char_pos_y_px, current_char_W, char_outer_height_px, char_frame_color, CHAR_SDL_OUTLINE_THICKNESS); break;
        case 'W': drawHollowW(gRenderer, char_pos_x_px - 130, char_pos_y_px, current_char_W, char_outer_height_px, char_frame_color, CHAR_SDL_OUTLINE_THICKNESS); break;
        case 'X': drawHollowX(gRenderer, char_pos_x_px - 75, char_pos_y_px, current_char_W, char_outer_height_px, char_frame_color, CHAR_SDL_OUTLINE_THICKNESS); break;
        case 'Y': drawHollowY(gRenderer, char_pos_x_px - 150, char_pos_y_px, current_char_W, char_outer_height_px, char_frame_color, CHAR_SDL_OUTLINE_THICKNESS); break;
        case 'Z': drawHollowZ(gRenderer, char_pos_x_px - 75, char_pos_y_px, current_char_W, char_outer_height_px, char_frame_color, CHAR_SDL_OUTLINE_THICKNESS); break;
        }

        drawText(gRenderer, "App name: Ung dung gia lap tap to", 10, 10, blackColor);
        drawText(gRenderer, "Nhom: 3, mon: Ky thuat do hoa", 10, 30, blackColor);

        int smallBoxSize = 100;
        int smallBoxPadding = 10;
        int smallBoxX = SCREEN_WIDTH - smallBoxSize - smallBoxPadding;
        int smallBoxY = smallBoxPadding;

        SDL_SetRenderDrawColor(gRenderer, 230, 230, 230, 255);
        SDL_Rect smallBoxRect = { smallBoxX, smallBoxY, smallBoxSize, smallBoxSize };
        SDL_RenderFillRect(gRenderer, &smallBoxRect);
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(gRenderer, &smallBoxRect);

        // Display bitmap character in the small preview box
        drawBitmapCharacterPreview(gRenderer, selectedChar, smallBoxX, smallBoxY, smallBoxSize, smallBoxSize, textColor);


        drawButton(gRenderer, prevBtnX, btnY, btnW, btnH, "Previous", prevBtnState, textColor);
        drawButton(gRenderer, nextBtnX, btnY, btnW, btnH, "Next", nextBtnState, greenColor);
        drawButton(gRenderer, pauseBtnX, pauseBtnY, pauseBtnW, btnH, pauseBtnText, pauseBtnState, textColor);

        drawText(gRenderer, "F1-F4:Color F5:Erase F6:Clear", 10, SCREEN_HEIGHT - 20, textColor);

        SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
        SDL_Rect visualDrawingBoundsRect = {
            DRAWING_ALLOWED_AREA_X_START, DRAWING_ALLOWED_AREA_Y_START,
            DRAWING_ALLOWED_AREA_X_END - DRAWING_ALLOWED_AREA_X_START, DRAWING_ALLOWED_AREA_Y_END - DRAWING_ALLOWED_AREA_Y_START
        };
        SDL_RenderDrawRect(gRenderer, &visualDrawingBoundsRect);
        SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_NONE);
        SDL_RenderPresent(gRenderer);
        SDL_Delay(16);
    }

    cleanupBGM();
    cleanupSFX();
    closeSDL();
    return 0;
}
