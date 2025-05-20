#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include <SDL_stdinc.h>
#include <SDL_timer.h>
#include <glm/fwd.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <SDL2/SDL.h>
#include "ParticleManager.hpp"
#include "glad/glad.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.hpp"
#include "Particle.hpp"
#include "Vertex.hpp"


// Globals
int gScreenWidth = 640;
int gScreenHeight = 480;
SDL_Window* gWindow = nullptr;
SDL_GLContext gOpenGLContext = nullptr;
bool gQuit = false;

// VAO - vertex array objects
GLuint gVertexArrayObject = 0;
// VBO - vertex buffer object
GLuint gVertexBufferObject = 0;
// IBO - index buffer objectg
GLuint gIndexBufferObject = 0;

bool gRotatingWithMouse = false;
bool gPanWithMouse = false;
bool gZoomWithMouse = false;

// shader program object
GLuint gPipelineProgram = 0;

glm::mat4 gmodelMatrix = glm::mat4(1.0f);
glm::mat4 gViewMatrix = glm::mat4(1.0f);

Camera gCamera = Camera(0.0f, 0.0f, 5.0f);

float gParticleSize = 0.1;

std::vector<GLuint> gIndexBufferData;

int gInstanceCnt = 300;

float gTime = 0;

ParticleManager gParticleManager = ParticleManager();



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

void UpdateCamera()
{

}

void Input()
{
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    SDL_Event e;
    while(SDL_PollEvent(&e))
    {
        switch(e.type)
        {
            case SDL_QUIT:
                std::cout << "Exiting\n";
                gQuit = true;
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (e.button.button == SDL_BUTTON_RIGHT)
                    gZoomWithMouse = true;
                else if (e.button.button == SDL_BUTTON_MIDDLE)
                    gPanWithMouse = true ;
                else if (e.button.button == SDL_BUTTON_LEFT)
                    gRotatingWithMouse = true;
                break;

            case SDL_MOUSEBUTTONUP:
                if (e.button.button == SDL_BUTTON_RIGHT)
                    gZoomWithMouse = false;
                else if (e.button.button == SDL_BUTTON_MIDDLE)
                    gPanWithMouse = false ;
                else if (e.button.button == SDL_BUTTON_LEFT)
                    gRotatingWithMouse = false;
                break;
            case SDL_MOUSEWHEEL:
                gCamera.changeRadius(static_cast<float>(e.wheel.y)/-3.0f);
                break;
            case SDL_MOUSEMOTION:
                if (gRotatingWithMouse)
                {
                    float dx = e.motion.xrel / -100.0f;
                    float dy = e.motion.yrel / -100.0f;

                    gCamera.rotateAroundCenter(dx, {0,1,0});
                    gCamera.rotateAroundCenter(dy,
                        gCamera.getRight() * glm::vec3(1.0f,0.0f,1.0f));
                }
                else if (gPanWithMouse)
                {
                    float dx = e.motion.xrel / -100.0f;
                    float dy = e.motion.yrel / -100.0f;


                    glm::vec3 up = gCamera.getUp()*dy;
                    glm::vec3 right = gCamera.getRight()*dx;

                    gCamera.changeCenter(up.x, up.y, up.z);
                    gCamera.changeCenter(right.x, right.y, right.z);
                }
                else if (gZoomWithMouse)
                {
                    float dx = e.motion.xrel / -50.0f;

                    gCamera.changeRadius(dx);
                }
                break;
        }
    }

    if(state[SDL_SCANCODE_F])
    {
       gCamera.setCenter(0.0, 0.0, 0.0); 
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

    // view matrix
    // gViewMatrix = glm::rotate(gViewMatrix, 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 gProjMatrix = glm::perspective(
        glm::radians(45.0f),                  // FOV
        float(gScreenWidth) / gScreenHeight, // aspect ratio
        0.1f,                                 // near plane
        100.0f                                // far plane
    );

    gTime += 0.01;
    gViewMatrix = gCamera.getViewMatrix();

    GLint viewMatrixLoc = glGetUniformLocation(gPipelineProgram, "uView");
    glUniformMatrix4fv( viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(gViewMatrix));

    GLint projMatrixLoc = glGetUniformLocation(gPipelineProgram, "uProj");
    glUniformMatrix4fv( projMatrixLoc, 1, GL_FALSE, glm::value_ptr(gProjMatrix));



}
void Draw()
{
    glBindVertexArray(gVertexArrayObject); 
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);

    
    GLenum renderMode;
    // renderMode = GL_TRIANGLES;
    renderMode = GL_LINE_LOOP;
    // renderMode = GL_POINTS;

    glDrawElementsInstanced(
        renderMode,
        gIndexBufferData.size(),
        GL_UNSIGNED_INT,
        0,
        gInstanceCnt
    );
    // glDrawArrays(GL_POINTS, 0, 10*10);
}

void SimulationStep(double deltaTime)
{
    gParticleManager.step(deltaTime);

    for(size_t i=0; i< gParticleManager.numParticles(); ++i)
    {
        Particle p = gParticleManager.getParticle(i);
        GLint offsetLoc = glGetUniformLocation(gPipelineProgram, std::string("offsets["+std::to_string(i)+"]").c_str());
        if(offsetLoc==-1)
        {
            continue;
        }
        // std::cout << " setting pos to: " << p.pos.x << " " << p.pos.y << " " << p.pos.z << "\n";
        glUniform3f( offsetLoc, p.pos.x, p.pos.y, p.pos.z);
    }
}

void MainLoop()
{
    Uint64 curCounter = SDL_GetPerformanceCounter();
    Uint64 prevCounter = 0;
    double deltaTime = 0;

    while(!gQuit){
        prevCounter = curCounter;
        curCounter = SDL_GetPerformanceCounter();
        deltaTime = static_cast<double>((curCounter - prevCounter) / static_cast<double>(SDL_GetPerformanceFrequency()));

        Input();

        SimulationStep(deltaTime);
        PreDraw();
        Draw();

        SDL_GL_SwapWindow(gWindow);

    }
}


template <typename T>
T fit(T value, T inMin, T inMax, T outMin, T outMax) {
    return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}


void VertexSpecification()
{
    // for cpu
    // std::vector<GLfloat> vertexPostion{
    //     // x    y    z
    //     -0.1f, -0.1, 0.0f, // bottom left
    //     0.1f, -0.1, 0.0f,  // bottom right 
    //     -0.1f, 0.1f, 0.0f, // top left
    //     0.1f, 0.1f, 0.0f, // top right
    // };
    // gIndexBufferData = 
    // {
    //     0,5,13,
    //     2,1,3
    // };

    std::vector<Vertex> vertices;
    int vertDivisions = 10;
    int horiDivisions = 10;
    int sphereTotalPts = vertDivisions*horiDivisions;
    float PI = 3.14159265358979323846;
    float radius = gParticleSize;

    // generate points
    for(int i =0; i<=vertDivisions; i++)
    {
        float lon = fit<float>(i, 0, vertDivisions, 0, PI);  
        for(int j = 0; j<horiDivisions; ++j)
        {
            float lat = fit<float>(j, 0, horiDivisions, 0, PI*2);
            float x = radius * sin(lon) * cos (lat);
            float y = radius * sin(lon) * sin (lat);
            float z = radius * cos(lon);

            Vertex vert;
            vert.pos.x = x;
            vert.pos.y = y;
            vert.pos.z = z;
            vert.normal = glm::normalize(vert.pos);
            vertices.push_back(vert);


            std::cout << "plotting at: " <<  x << " " << y << " " << z << "\n";
        }
    }

    // connect mesh
    for (int i = 0; i < vertDivisions; ++i) {
        for (int j = 0; j < horiDivisions; ++j) {
            int current = i * horiDivisions + j;


            // compute indices
            int next = current + horiDivisions;
            int nextRight = (i+1)*horiDivisions    +    (j+1)%horiDivisions;
            int currentRight = (i)*horiDivisions    +    (j+1)%horiDivisions;



            // first triangle 
            gIndexBufferData.push_back(current);
            gIndexBufferData.push_back(nextRight);
            gIndexBufferData.push_back(next);

            // second triangle
            gIndexBufferData.push_back(current);
            gIndexBufferData.push_back(currentRight);
            gIndexBufferData.push_back(nextRight);
        }
    }



    // for gpu
    glGenVertexArrays(1, &gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);

    glGenBuffers(1, &gVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size()*sizeof(Vertex),
        vertices.data(),
        GL_STATIC_DRAW
    );

    glGenBuffers(1, &gIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferObject);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        gIndexBufferData.size()*sizeof(GLuint),
        gIndexBufferData.data(),
        GL_STATIC_DRAW
    );

    GLsizei stride = sizeof(Vertex);
    
    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, // index
        3, // size
        GL_FLOAT, // type
        GL_FALSE, // normalized
        stride, // stride
        (void*)offsetof(Vertex, pos)
    );
    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, // index
        3, // size
        GL_FLOAT, // type
        GL_FALSE, // normalized
        stride, // stride
        (void*)offsetof(Vertex, normal)
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
        "uniform mat4 uView;\n"
        "uniform mat4 uProj;\n"
        "uniform vec3 offsets["+std::to_string(gInstanceCnt)+"];\n"
        "void main()\n"
        "{\n"
        "   vec3 offset = offsets[gl_InstanceID];\n"
        "   gl_Position = uProj * uView * vec4(position.xyz+offset, position.w);\n"
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
    std::cout << "cleaning up\n";

    glDeleteProgram(gPipelineProgram);
    glDeleteBuffers(1, &gVertexBufferObject);
    glDeleteBuffers(1, &gIndexBufferObject);
    glDeleteVertexArrays(1, &gVertexArrayObject);

    SDL_GL_DeleteContext(gOpenGLContext);

    SDL_DestroyWindow(gWindow);

    SDL_Quit();
}

void InitializeParticles()
{
    float size = 0.1f;
    int rows = floor(sqrt(gInstanceCnt));
    for(int i=0; i<gInstanceCnt; i++)
    {
        Particle particle;
        particle.pos.x = i/rows*size;
        particle.pos.y = 3+i/3.0f;
        particle.pos.z = (i%rows)*size;
        particle.rad = gParticleSize;
        gParticleManager.addParticle(particle);
    }
}

int main(){
    InitializeProgram();
    InitializeParticles();

    VertexSpecification();

    CreateGraphicsPipeline();

    MainLoop();

    CleanUp();
}
