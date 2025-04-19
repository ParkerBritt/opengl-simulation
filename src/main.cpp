#include <iostream>
#include <SDL.h>

#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600

int main(){
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Basic C++ SDL project",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool quit = false;

    while(!quit){

        SDL_Event e;

        SDL_WaitEvent(&e);

        if(e.type == SDL_QUIT)
        {
            quit = true;
        }

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);


    }

    SDL_Quit();
}
