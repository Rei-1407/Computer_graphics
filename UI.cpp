#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <string>
#include <vector>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

enum ButtonState {
    BUTTON_NORMAL,
    BUTTON_HOVERED,
    BUTTON_CLICKED
};

// ==== PHẦN AUDIO SDL2 CORE ==== //
SDL_AudioDeviceID audioDevice = 0;
Uint8* bgmBuffer = nullptr;
Uint32 bgmLength = 0;
SDL_AudioSpec bgmSpec;
bool isMusicPaused = false;

// ==== SOUND EFFECT ==== //
Uint8* sfxBuffer = nullptr;
Uint32 sfxLength = 0;
SDL_AudioSpec sfxSpec;

bool initBGM(const char* filename) {
    if (SDL_LoadWAV(filename, &bgmSpec, &bgmBuffer, &bgmLength) == NULL) {
        SDL_Log("Failed to load wav: %s", SDL_GetError());
        return false;
    }
    bgmSpec.callback = NULL;
    audioDevice = SDL_OpenAudioDevice(NULL, 0, &bgmSpec, NULL, 0);
    if (!audioDevice) {
        SDL_Log("Failed to open audio: %s", SDL_GetError());
        SDL_FreeWAV(bgmBuffer);
        return false;
    }
    SDL_QueueAudio(audioDevice, bgmBuffer, bgmLength);
    SDL_PauseAudioDevice(audioDevice, 0); // Bắt đầu phát
    return true;
}

bool initSFX(const char* filename) {
    if (SDL_LoadWAV(filename, &sfxSpec, &sfxBuffer, &sfxLength) == NULL) {
        SDL_Log("Failed to load sound effect: %s", SDL_GetError());
        return false;
    }
    return true;
}

// Hàm phát hiệu ứng âm thanh (sound effect) khi nhấn button
void playSFX() {
    // Mở audio device tạm cho sound effect
    SDL_AudioDeviceID sfxDevice = SDL_OpenAudioDevice(NULL, 0, &sfxSpec, NULL, 0);
    if (!sfxDevice) {
        SDL_Log("Failed to open audio device for SFX: %s", SDL_GetError());
        return;
    }
    SDL_QueueAudio(sfxDevice, sfxBuffer, sfxLength);
    SDL_PauseAudioDevice(sfxDevice, 0); // Bắt đầu phát

    // Đợi phát xong rồi đóng device (tính thời gian phát)
    Uint32 ms = (Uint32)(1000.0 * sfxLength / (sfxSpec.freq * sfxSpec.channels * (SDL_AUDIO_BITSIZE(sfxSpec.format) / 8)));
    SDL_Delay(ms);
    SDL_CloseAudioDevice(sfxDevice);
}

void playOrLoopBGM() {
    if (!audioDevice || !bgmBuffer || bgmLength == 0) return;
    Uint32 queued = SDL_GetQueuedAudioSize(audioDevice);
    if (!isMusicPaused && queued == 0) {
        SDL_ClearQueuedAudio(audioDevice);
        SDL_QueueAudio(audioDevice, bgmBuffer, bgmLength);
    }
}

void cleanupBGM() {
    if (audioDevice) {
        SDL_ClearQueuedAudio(audioDevice);
        SDL_CloseAudioDevice(audioDevice);
        audioDevice = 0;
    }
    if (bgmBuffer) {
        SDL_FreeWAV(bgmBuffer);
        bgmBuffer = nullptr;
        bgmLength = 0;
    }
}

// ==== DỌN DẸP SFX ==== //
void cleanupSFX() {
    if (sfxBuffer) {
        SDL_FreeWAV(sfxBuffer);
        sfxBuffer = nullptr;
        sfxLength = 0;
    }
}

// ==== VẼ NÚT ==== //
void drawButton(SDL_Renderer* renderer, int x, int y, int w, int h, 
                const std::string& text, ButtonState state, SDL_Color textColor) {
    SDL_Color btnColor;
    switch(state) {
        case BUTTON_HOVERED: btnColor = {210, 210, 210, 255}; break;
        case BUTTON_CLICKED: btnColor = {190, 190, 190, 255}; break;
        default: btnColor = {255, 255, 255, 255}; break;
    }
    roundedBoxRGBA(renderer, x, y, x + w, y + h, 8, btnColor.r, btnColor.g, btnColor.b, btnColor.a);
    if(state == BUTTON_HOVERED || state == BUTTON_CLICKED) {
        roundedRectangleRGBA(renderer, x, y, x + w, y + h, 8, 0, 0, 0, 255);
    }
    int textX = x + (w - text.length() * 8) / 2;
    int textY = y + (h - 8) / 2;
    stringRGBA(renderer, textX, textY, text.c_str(), textColor.r, textColor.g, textColor.b, textColor.a);
}

// ==== VẼ CHỮ ==== //
void drawText(SDL_Renderer* renderer, const std::string& text, int x, int y, SDL_Color color) {
    stringRGBA(renderer, x, y, text.c_str(), color.r, color.g, color.b, color.a);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    // ==== Khởi tạo nhạc nền ====
    if (!initBGM("sounds/bgm.wav")) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Audio Error", "Không thể mở file bgm.wav", NULL);
        // Vẫn chạy chương trình, chỉ không có nhạc
    }

    // ==== Khởi tạo hiệu ứng âm thanh ====
    if (!initSFX("sounds/cursor.wav")) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Audio Error", "Không thể mở file cursor.wav", NULL);
        // Có thể bỏ qua, không có hiệu ứng click
    }

    SDL_Window* window = SDL_CreateWindow("Draw Letter Game!", SDL_WINDOWPOS_CENTERED, 
                                        SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Khởi tạo 26 chữ cái
    std::vector<std::string> chars;
    for (char c = 'A'; c <= 'Z'; ++c) {
        chars.push_back(std::string(1, c));
    }
    int currentChar = 0;

    SDL_Color bgColor = {220, 220, 220, 255};
    SDL_Color textColor = {0, 0, 0, 255};
    SDL_Color redColor = {255, 0, 0, 255};
    SDL_Color greenColor = {0, 128, 0, 255};

    // Layout constants
    int centerBoxX = (SCREEN_WIDTH - 400) / 2;
    int centerBoxY = (SCREEN_HEIGHT - 400) / 2;
    int btnW = 120;
    int btnH = 50;
    int btnY = centerBoxY + 400 + 20;
    int prevBtnX = centerBoxX;
    int nextBtnX = centerBoxX + 400 - btnW;

    // Nút pause
    const int pauseBtnW = 80;
    const int pauseBtnX = pauseBtnW - 40;
    const int pauseBtnY = 60;
    ButtonState pauseBtnState = BUTTON_NORMAL;
    std::string pauseBtnText = "Pause";

    // Trạng thái nút
    ButtonState prevBtnState = BUTTON_NORMAL;
    ButtonState nextBtnState = BUTTON_NORMAL;
    bool mouseDown = false;

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        // Lặp nhạc nếu hết
        playOrLoopBGM();

        // Cập nhật trạng thái chuột
        int mx, my;
        Uint32 mouseState = SDL_GetMouseState(&mx, &my);
        bool leftDown = (mouseState & SDL_BUTTON_LMASK) != 0;

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) mouseDown = true;
            }
            else if (e.type == SDL_MOUSEBUTTONUP) {
                if (e.button.button == SDL_BUTTON_LEFT) mouseDown = false;
            }
        }

        // Kiểm tra hover và click các nút
        bool prevHovered = (mx >= prevBtnX && mx <= prevBtnX + btnW && my >= btnY && my <= btnY + btnH);
        bool nextHovered = (mx >= nextBtnX && mx <= nextBtnX + btnW && my >= btnY && my <= btnY + btnH);
        bool pauseHovered = (mx >= pauseBtnX && mx <= pauseBtnX + pauseBtnW && my >= pauseBtnY && my <= pauseBtnY + 30);

        // Previous
        if(prevHovered) {
            prevBtnState = leftDown ? BUTTON_CLICKED : BUTTON_HOVERED;
            if(!leftDown && mouseDown) {
                if(currentChar > 0) currentChar--;
                playSFX(); // Phát hiệu ứng âm thanh
                mouseDown = false;
            }
        } else prevBtnState = BUTTON_NORMAL;

        // Next
        if(nextHovered) {
            nextBtnState = leftDown ? BUTTON_CLICKED : BUTTON_HOVERED;
            if(!leftDown && mouseDown) {
                if(currentChar < (int)chars.size() - 1) currentChar++;
                playSFX(); // Phát hiệu ứng âm thanh
                mouseDown = false;
            }
        } else nextBtnState = BUTTON_NORMAL;

        // Pause/Resume
        if(pauseHovered) {
            if(!leftDown && mouseDown) {
                isMusicPaused = !isMusicPaused;
                if(isMusicPaused) SDL_PauseAudioDevice(audioDevice, 1);
                else SDL_PauseAudioDevice(audioDevice, 0);
                playSFX(); // Phát hiệu ứng âm thanh
                mouseDown = false;
            }
            pauseBtnState = leftDown ? BUTTON_CLICKED : BUTTON_HOVERED;
        } else pauseBtnState = BUTTON_NORMAL;
        pauseBtnText = isMusicPaused ? "Resume" : "Pause";

        // ==== VẼ GIAO DIỆN ==== //
        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_RenderClear(renderer);

        // Góc trên trái
        drawText(renderer, "App name: Ung dung gia lap tap to", 10, 10, redColor);
        drawText(renderer, "Nhom: 3, mon: Ky thuat do hoa", 10, 30, redColor);

        // Khung chính
        boxRGBA(renderer, centerBoxX, centerBoxY, centerBoxX + 400, centerBoxY + 400, 255, 255, 255, 255);
        rectangleRGBA(renderer, centerBoxX, centerBoxY, centerBoxX + 400, centerBoxY + 400, 0, 0, 0, 255);
        int textWidth = 8 * chars[currentChar].size();
        int textX = centerBoxX + (400 - textWidth) / 2;
        int textY = centerBoxY + (400 - 8) / 2;
        drawText(renderer, chars[currentChar], textX, textY, textColor);

        // Khung nhỏ góc trên phải
        int smallBoxX = SCREEN_WIDTH - 110;
        int smallBoxY = 10;
        boxRGBA(renderer, smallBoxX, smallBoxY, smallBoxX + 100, smallBoxY + 100, 230, 230, 230, 255);
        rectangleRGBA(renderer, smallBoxX, smallBoxY, smallBoxX + 100, smallBoxY + 100, 0, 0, 0, 255);
        drawText(renderer, chars[currentChar], smallBoxX + 40, smallBoxY + 40, textColor);

        // Nút Previous/Next
        drawButton(renderer, prevBtnX, btnY, btnW, btnH, "Previous", prevBtnState, textColor);
        drawButton(renderer, nextBtnX, btnY, btnW, btnH, "Next", nextBtnState, greenColor);

        // Nút Pause/Resume
        drawButton(renderer, pauseBtnX, pauseBtnY, pauseBtnW, 30, pauseBtnText, pauseBtnState, textColor);

        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

    // Dọn dẹp
    cleanupBGM();
    cleanupSFX();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
