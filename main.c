#include <SDL2/SDL.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create a window (Full screen or windowed)
    SDL_Window* window = SDL_CreateWindow("CoreDE", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        800, 600, SDL_WINDOW_SHOWN);
    
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a hardware-accelerated renderer with vsync enabled (kills screen tearing automatically!)
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    int running = 1;
    SDL_Event e;
    int mouse_x = 400, mouse_y = 300;

    while (running) {
        // Handle input events (mouse, keyboard)
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                running = 0;
            } else if (e.type == SDL_MOUSEMOTION) {
                mouse_x = e.motion.x;
                mouse_y = e.motion.y;
            }
        }

        // 1. Clear screen (Desktop Background - Dark Navy)
        SDL_SetRenderDrawColor(renderer, 30, 40, 60, 255);
        SDL_RenderClear(renderer);

        // 2. Draw Taskbar at the bottom (Height: 40px, Dark Charcoal)
        SDL_Rect taskbar = {0, 560, 800, 40};
        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        SDL_RenderFillRect(renderer, &taskbar);

        // Taskbar top accent line
        SDL_SetRenderDrawColor(renderer, 255, 128, 0, 255);
        SDL_RenderDrawLine(renderer, 0, 560, 800, 560);

        // 3. Draw Mouse Cursor (Simple 10x10 white square)
        SDL_Rect cursor = {mouse_x, mouse_y, 10, 10};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &cursor);

        // 4. Present the back-buffer to the screen (VSync keeps it buttery smooth)
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
