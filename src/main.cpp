#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <SDL_keyboard.h>
#include <glm/fwd.hpp>
#include <iostream>
#include <SDL2/SDL.h>
#include "glad/glad.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


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

// shader program object
GLuint gPipelineProgram = 0;

glm::mat4 gmodelMatrix = glm::mat4(1.0f);
glm::mat4 gViewMatrix = glm::mat4(1.0f);
glm::vec3 gCamPos = glm::vec3(0.0f,0.0f,3.0f);

int gtotalIndices = 0;

std::vector<GLuint> gIndexBufferData;

int gInstanceCnt = 1;

float gTime = 0;



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

        const Uint8 *state = SDL_GetKeyboardState(NULL);
        if(state[SDL_SCANCODE_SPACE])
        {
            gCamPos+=glm::vec3(0.0f,0.1f,0.0f); 
        }
        if(state[SDL_SCANCODE_C])
        {
            gCamPos+=glm::vec3(0.0f,-0.1f,0.0f); 
        }
        if(state[SDL_SCANCODE_LEFT])
        {
            gCamPos+=glm::vec3(-0.1f,0.0f,0.0f); 
        }
        if(state[SDL_SCANCODE_RIGHT])
        {
            gCamPos+=glm::vec3(0.1f,0.0f,0.0f); 
        }
        if(state[SDL_SCANCODE_UP])
        {
            gCamPos+=glm::vec3(0.0f,0.0f,0.1f); 
        }
        if(state[SDL_SCANCODE_DOWN])
        {
            gCamPos+=glm::vec3(0.0f,0.0f,-0.1f); 
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

    // view matrix
    // gViewMatrix = glm::rotate(gViewMatrix, 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 gProjMatrix = glm::perspective(
        glm::radians(45.0f),                  // FOV
        float(gScreenWidth) / gScreenHeight, // aspect ratio
        0.1f,                                 // near plane
        100.0f                                // far plane
    );

    gTime += 0.01;
    gViewMatrix = glm::lookAt(
        gCamPos,
        glm::vec3(0.0f,0.0f,0.0f),
        glm::vec3(0.0f,1.0f,0.0f)
    );

    GLint viewMatrixLoc = glGetUniformLocation(gPipelineProgram, "uView");
    glUniformMatrix4fv( viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(gViewMatrix));

    GLint projMatrixLoc = glGetUniformLocation(gPipelineProgram, "uProj");
    glUniformMatrix4fv( projMatrixLoc, 1, GL_FALSE, glm::value_ptr(gProjMatrix));

    glm::vec3 translations[gInstanceCnt];
    int index = 0;
    float offset = 0.1f;
    for(size_t i; i<gInstanceCnt; ++i)
    {
        glm::vec3 translation;
        translation.x = i;
        translations[i] = translation;
    }

    for(size_t i=0; i< gInstanceCnt; ++i)
    {
        GLint offsetLoc = glGetUniformLocation(gPipelineProgram, std::string("offsets["+std::to_string(i)+"]").c_str());
        if(offsetLoc==-1)
        {
            continue;
        }
        glUniform3f( offsetLoc, translations[i].x, translations[i].y, translations[i].z);
    }


}
void Draw()
{
    glBindVertexArray(gVertexArrayObject); 
    glBindBuffer(GL_ARRAY_BUFFER, gVertexArrayObject);

    
    GLenum renderMode;
    renderMode = GL_TRIANGLES;
    // renderMode = GL_LINE_LOOP;
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

void MainLoop()
{
    while(!gQuit){
        Input();

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

    std::vector<GLfloat> vertexPostion;
    int vertDivisions = 5;
    int horiDivisions = 5;
    int sphereTotalPts = vertDivisions*horiDivisions;
    float PI = 3.14159265358979323846;
    float radius = 0.5;

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

            vertexPostion.push_back(x);
            vertexPostion.push_back(y);
            vertexPostion.push_back(z);

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
        vertexPostion.size()*sizeof(GLfloat),
        vertexPostion.data(),
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
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, // index
        3, // size
        GL_FLOAT, // type
        GL_FALSE, // normalized
        sizeof(GLfloat)*3, // stride
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
        "in vec4 position;\n"
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

int main(){
    InitializeProgram();

    VertexSpecification();

    CreateGraphicsPipeline();

    MainLoop();

    CleanUp();
}
