#include <glad/glad.h>
#include <SDL3/SDL.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <iostream>
#include <math.h>
#include <string>

#include "Shader.h"
#include "Model.h"
#include "Camera.h"

/* Prototypes */
void FramebufferSize_Callback(SDL_Window* window, int width, int height);
bool ProcessInput(SDL_Window* window, SDL_Event& event, Camera& camera);

glm::vec3 pointLightPositions[] = {
	glm::vec3( 0.7f,  0.2f,  2.0f),
	glm::vec3( 2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3( 0.0f,  0.0f, -3.0f)
};

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

int main(int argc, char* argv[])
{
    /****************************************************/
    /**************** SDL INITIALISATION ****************/
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "Failed to initialise SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

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

    /**************** SDL INITIALISATION ****************/
    /****************************************************/

    glViewport(0, 0, 800, 600);

    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

    Shader shaderProgram("../../shaders/shader.vs", "../../shaders/shader.fs");
    shaderProgram.Use();

    // DirectionalLight
    shaderProgram.SetVec3("directionalLight.direction", -0.2f, -1.0f, -0.3f);
    shaderProgram.SetVec3("directionalLight.ambient", 0.05f, 0.05f, 0.05f);
    shaderProgram.SetVec3("directionalLight.diffuse", 0.05f, 0.05f, 0.05f);
    shaderProgram.SetVec3("directionalLight.specular", 0.1f, 0.1f, 0.1f);
    // Point Light 1
    shaderProgram.SetVec3("pointLight[0].position", pointLightPositions[0]);
    shaderProgram.SetFloat("pointLight[0].constantAttenuation",  1.0f);
    shaderProgram.SetFloat("pointLight[0].linearAttenuation",    0.09f);
    shaderProgram.SetFloat("pointLight[0].quadraticAttenuation", 0.032f);
    shaderProgram.SetVec3("pointLight[0].ambient", 0.2f, 0.2f, 0.2f);
    shaderProgram.SetVec3("pointLight[0].diffuse", 0.7f, 0.7f, 0.7f);
    shaderProgram.SetVec3("pointLight[0].specular", 1.0f, 1.0f, 1.0f);
    // Point Light 2
    shaderProgram.SetVec3("pointLight[1].position", pointLightPositions[1]);
    shaderProgram.SetFloat("pointLight[1].constantAttenuation",  1.0f);
    shaderProgram.SetFloat("pointLight[1].linearAttenuation",    0.09f);
    shaderProgram.SetFloat("pointLight[1].quadraticAttenuation", 0.032f);
    shaderProgram.SetVec3("pointLight[1].ambient", 0.2f, 0.2f, 0.2f);
    shaderProgram.SetVec3("pointLight[1].diffuse", 0.7f, 0.7f, 0.7f);
    shaderProgram.SetVec3("pointLight[1].specular", 1.0f, 1.0f, 1.0f);
    // Point Light 3
    shaderProgram.SetVec3("pointLight[2].position", pointLightPositions[2]);
    shaderProgram.SetFloat("pointLight[2].constantAttenuation",  1.0f);
    shaderProgram.SetFloat("pointLight[2].linearAttenuation",    0.09f);
    shaderProgram.SetFloat("pointLight[2].quadraticAttenuation", 0.032f);
    shaderProgram.SetVec3("pointLight[2].ambient", 0.2f, 0.2f, 0.2f);
    shaderProgram.SetVec3("pointLight[2].diffuse", 0.7f, 0.7f, 0.7f);
    shaderProgram.SetVec3("pointLight[2].specular", 1.0f, 1.0f, 1.0f);
    // Point Light 4
    shaderProgram.SetVec3("pointLight[3].position", pointLightPositions[3]);
    shaderProgram.SetFloat("pointLight[3].constantAttenuation",  1.0f);
    shaderProgram.SetFloat("pointLight[3].linearAttenuation",    0.09f);
    shaderProgram.SetFloat("pointLight[3].quadraticAttenuation", 0.032f);
    shaderProgram.SetVec3("pointLight[3].ambient", 0.2f, 0.2f, 0.2f);
    shaderProgram.SetVec3("pointLight[3].diffuse", 0.7f, 0.7f, 0.7f);
    shaderProgram.SetVec3("pointLight[3].specular", 1.0f, 1.0f, 1.0f);

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    /**************** Render Loop ****************/
    bool running = true;
    SDL_Event event;
    glEnable(GL_DEPTH_TEST);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    Model backpack("../../resources/Backpack/backpack.obj");

    while (running)
    {
        float currentFrame = (double)SDL_GetTicks() / 1000;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        running = ProcessInput(window, event, camera);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        viewMatrix = camera.GetViewMatrix();
        projectionMatrix = glm::perspective(glm::radians(camera.GetFOV()), 800.0f / 600.0f, 0.1f, 100.0f);
        shaderProgram.SetVec3("viewPos", camera.GetPosition());

        shaderProgram.Use();

        unsigned int modelMatrixUniformLocation = glGetUniformLocation(shaderProgram.GetID(), "modelMatrix");
        unsigned int viewMatrixUniformLocation = glGetUniformLocation(shaderProgram.GetID(), "viewMatrix");
        unsigned int projectionMatrixUniformLocation = glGetUniformLocation(shaderProgram.GetID(), "projectionMatrix");
        glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(viewMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        backpack.Draw(shaderProgram);

        SDL_GL_SwapWindow(window);
    }

    // Cleanup before exiting.
    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void FramebufferSize_Callback(SDL_Window* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

bool ProcessInput(SDL_Window* window, SDL_Event& event, Camera& camera)
{
    const float cameraSpeed = 2.5f * deltaTime;

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
                FramebufferSize_Callback(window, width, height);
            }
            break;

            case SDL_EVENT_MOUSE_MOTION:
            {
                camera.ProcessMouseMovement(event.motion.xrel, event.motion.yrel, GL_TRUE);
            }
            break;

            case SDL_EVENT_MOUSE_WHEEL:
            {
                camera.ProcessMouseScroll((float)event.wheel.y);
            }
        }
    }

    if (keyboardState[SDL_SCANCODE_W]) camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keyboardState[SDL_SCANCODE_S]) camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keyboardState[SDL_SCANCODE_A]) camera.ProcessKeyboard(LEFT, deltaTime);
    if (keyboardState[SDL_SCANCODE_D]) camera.ProcessKeyboard(RIGHT, deltaTime);

    return true;
}