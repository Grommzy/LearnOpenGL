#include <glad/glad.h>
#include <SDL3/SDL.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <stb_image.h>

#include <iostream>
#include <math.h>
#include <string>
#include <map>

#include "Shader.h"
#include "Model.h"
#include "Camera.h"

/* Prototypes */
bool ProcessInput(SDL_Window* window, SDL_Event& event);

double deltaTime   = 0.0;
double deltaTimeMS = 0.0;
Uint64 lastHiRezCount = SDL_GetPerformanceCounter();

double runningSeconds = 0.0;

int main(int argc, char* argv[])
{
    //====================================================[ SDL_INITIALIZATION ]====================================================\\ 

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "Failed to initialise SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // Create an SDL window with an OpenGL context flag
    SDL_Window* window = SDL_CreateWindow("LearnOpenGL", 1920, 1080, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MOUSE_GRABBED | SDL_WINDOW_MOUSE_RELATIVE_MODE);
    if (!window)
    {
        std::cout << "Failed to create SDL window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }
    SDL_SetWindowRelativeMouseMode(window, true);

    // Create an OpenGL context associated with the window
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (glContext == nullptr)
    {
        std::cout << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // GLAD function pointers must be loaded for every context that utilises OpenGL.
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        std::cout << "Failed to initialise GLAD function pointers: " << SDL_GetError() << std::endl;
        SDL_GL_DestroyContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    //====================================================[ OPENGL SHADER_INITIALIZATION ]====================================================\\ 

    glViewport(0, 0, 1920, 1080);
    Shader shaderProgram("../../shaders/shader.vs", "../../shaders/shader.gs", "../../shaders/shader.fs");
    Model backpack("../../resources/Backpack/backpack.obj");
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

    //====================================================[ RENDER_LOOP ]====================================================\\ 

    bool running = true;
    SDL_Event event;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (running)
    {
        // Using the High Resolution Counter to get a more accurate delta-time.
        Uint64 currentHiRezCount = SDL_GetPerformanceCounter();

        deltaTime   = (double)(currentHiRezCount - lastHiRezCount) / SDL_GetPerformanceFrequency();
        deltaTimeMS = deltaTime * 1000;

        lastHiRezCount = currentHiRezCount;
        double fps     = 1.0 / deltaTime;

        runningSeconds = (double)SDL_GetTicks() / 1000;

        running = ProcessInput(window, event);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        shaderProgram.Use();

        glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera.GetFOV()), 1920.0f / 1080.0f, 1.0f, 100.0f);
        glm::mat4 viewMatrix = camera.GetViewMatrix();
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        shaderProgram.SetMat4("projectionMatrix", projectionMatrix);
        shaderProgram.SetMat4("viewMatrix", viewMatrix);
        shaderProgram.SetMat4("modelMatrix", modelMatrix);

        shaderProgram.SetFloat("time", (float)runningSeconds);
        backpack.Draw(shaderProgram);

        SDL_GL_SwapWindow(window);
    }

    // Cleanup before exiting.
    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

bool ProcessInput(SDL_Window* window, SDL_Event& event)
{
    const bool* keyboardState = SDL_GetKeyboardState(NULL);
    while(SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_EVENT_QUIT:
            {
                return false;
            }

            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            {
                int width = event.window.data1;
                int height = event.window.data2;
                glViewport(0, 0, width, height);
            }
            break;
        }
    }

    return true;
}