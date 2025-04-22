#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <glad/glad.h>


// Globals
int gScreenWidth = 640;
int gScreenHeight = 480;
SDL_Window* gWindow = nullptr;
SDL_GLContext gOpenGLContext = nullptr;
bool gQuit = false;


void GetOpenGLVersionInfo(){
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
}

void InitializeProgram(){
    if(SDL_INIT_VIDEO < 0){
        std::cout << "SDL_INIT_VIDEO failed\n";
        exit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);


    gWindow = SDL_CreateWindow("Simulation", 
                     SDL_WINDOWPOS_UNDEFINED, // x pos
                     SDL_WINDOWPOS_UNDEFINED, // y pos
                     gScreenWidth,            // width
                     gScreenHeight,           // height
                     SDL_WINDOW_OPENGL        // flags
                     );

    if(gWindow == nullptr){
        std::cout << "SDL_CreateWindow failed\n";
    }

    gOpenGLContext = SDL_GL_CreateContext(gWindow);

    if(gOpenGLContext == nullptr){
        std::cout << "SDL_GL_CreateContext failed\n";
    }

    // init glad lib
    if(!gladLoadGLLoader(SDL_GL_GetProcAddress)){
        std::cout << "glad was not initialized" << std::endl;
        exit(1);
    }

    GetOpenGLVersionInfo();
}


void Input()
{
    SDL_Event e;

    while(SDL_PollEvent(&e) !=0){
        if(e.type == SDL_QUIT){
            std::cout << "Exiting\n";
            gQuit = true;
        }
    }

}
void PreDraw()
{

}
void Draw()
{

}

void MainLoop()
{
    while(!gQuit){
        Input();

        PreDraw();
        Draw();

        SDL_GL_SwapWindow(gWindow);

    }
}

void CleanUp(){
    SDL_DestroyWindow(gWindow);

    SDL_Quit();
}

int main(){
    InitializeProgram();

    MainLoop();

    CleanUp();
}
