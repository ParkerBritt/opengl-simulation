#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <iostream>
#include <SDL2/SDL.h>
#include "glad/glad.h"
#include <vector>


// Globals
int gScreenWidth = 640;
int gScreenHeight = 480;
SDL_Window* gWindow = nullptr;
SDL_GLContext gOpenGLContext = nullptr;
bool gQuit = false;

// VAO
GLuint gVertexArrayObject = 0;
// VBO
GLuint gVertexBufferObject = 0;

// shader program object
GLuint gPipelineProgram = 0;

void GetOpenGLVersionInfo(){
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
    std::cout << "Version: " << glGetString(GL_VERSION) << "\n";
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
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
// set opengl state
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glViewport(0,0,gScreenWidth, gScreenHeight);
    glClearColor(0.18f, 0.18f, 0.18f, 1.f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(gPipelineProgram);
}
void Draw()
{
    glBindVertexArray(gVertexArrayObject); 
    glBindBuffer(GL_ARRAY_BUFFER, gVertexArrayObject);

    glDrawArrays(GL_TRIANGLES, 0, 3);
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

void VertexSpecification()
{
    // for cpu
    const std::vector<GLfloat> vertexPostion{
        // x    y    z
        -0.8f, -0.8f, 0.0f,
        0.8f, -0.8f, 0.0f,
        0.0f, 0.8f, 0.0f
    };

    // for gpu
    glGenVertexArrays(1, &gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);

    glGenBuffers(1, &gVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertexPostion.size()*sizeof(GLfloat),
        vertexPostion.data(),
        GL_STATIC_DRAW
    );
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, // index
        3, // size
        GL_FLOAT, // type
        GL_FALSE, // normalized
        0, // stride
        (void*)0
    );

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
}

GLuint CompileShader(GLuint type, const std::string& _shaderSource){
    GLuint shaderObject;

    if(type == GL_VERTEX_SHADER)
    {
        shaderObject = glCreateShader(GL_VERTEX_SHADER);
    }
    else if(type == GL_FRAGMENT_SHADER)
    {
        shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    }

    const char* src = _shaderSource.c_str();
    glShaderSource(
        shaderObject,
        1,
        &src,
        nullptr
    );
    glCompileShader(shaderObject);

    return shaderObject;
}

GLuint CreateShaderProgram(const std::string& _vertexShaderSource, const std::string _fragmentShaderSource)
{
    GLuint programObject = glCreateProgram();

    GLuint vertShader = CompileShader(GL_VERTEX_SHADER, _vertexShaderSource);
    GLuint fragShader = CompileShader(GL_FRAGMENT_SHADER, _fragmentShaderSource);

    glAttachShader(programObject, vertShader);
    glAttachShader(programObject, fragShader);

    glLinkProgram(programObject);

    glValidateProgram(programObject);

    return programObject;
}

void CreateGraphicsPipeline()
{
    std::string vsSource =
        "#version 410 core\n"
        "in vec4 position;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(position.x, position.y, position.z, position.w);\n"
        "}\n"
    ;
    std::string fsSource =
        "#version 410 core\n"
        "out vec4 color;\n"
        "void main()\n"
        "{\n"
        "   color = vec4(1.0f, 0.5, 0.0f, 1.0f);\n"
        "}\n"
    ;
    gPipelineProgram = CreateShaderProgram(vsSource, fsSource);
}


void CleanUp(){
    SDL_DestroyWindow(gWindow);

    SDL_Quit();
}

int main(){
    InitializeProgram();

    VertexSpecification();

    CreateGraphicsPipeline();

    MainLoop();

    CleanUp();
}
