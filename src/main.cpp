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
void FramebufferSize_Callback(SDL_Window* window, int width, int height);
bool ProcessInput(SDL_Window* window, SDL_Event& event, Camera& camera);
unsigned int LoadCubeMap(std::vector<std::string> faces);

std::vector<std::string> cubeMapFaces
{
    "../../resources/skybox/right.jpg",
    "../../resources/skybox/left.jpg",
    "../../resources/skybox/top.jpg",
    "../../resources/skybox/bottom.jpg",
    "../../resources/skybox/front.jpg",
    "../../resources/skybox/back.jpg"
};

glm::vec3 pointLightPositions[] = {
	glm::vec3( 0.7f,  0.2f,  2.0f),
	glm::vec3( 2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3( 0.0f,  0.0f, -3.0f)
};

glm::vec3 windowPanePositions[] = {
    glm::vec3( 0.0f,  0.0f,  2.0f),
    glm::vec3( 0.0f,  0.0f,  3.0f),
    glm::vec3( 1.0f,  0.0f,  4.0f),
    glm::vec3(-1.0f,  0.0f,  1.0f)
};

float frameBufferQuad[] = {  
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
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

    /**************** SDL INITIALISATION ****************/
    /****************************************************/

    glViewport(0, 0, 1920, 1080);

    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

    Shader shaderProgram("../../shaders/shader.vs", "../../shaders/shader.fs");
    Shader blendingShader("../../shaders/shader.vs", "../../shaders/blending.fs");
    Shader framebufferShader("../../shaders/framebufferScreen.vs", "../../shaders/framebufferScreen.fs");
    Shader skyboxShader("../../shaders/skybox.vs", "../../shaders/skybox.fs");
    Shader reflectiveShader("../../shaders/reflectiveMaterial.vs", "../../shaders/reflectiveMaterial.fs");
    Shader refractiveShader("../../shaders/refractiveMaterial.vs", "../../shaders/refractiveMaterial.fs");
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
    shaderProgram.SetVec3("pointLight[0].diffuse", 0.0f, 0.7f, 0.7f);
    shaderProgram.SetVec3("pointLight[0].specular", 1.0f, 1.0f, 1.0f);
    // Point Light 2
    shaderProgram.SetVec3("pointLight[1].position", pointLightPositions[1]);
    shaderProgram.SetFloat("pointLight[1].constantAttenuation",  1.0f);
    shaderProgram.SetFloat("pointLight[1].linearAttenuation",    0.09f);
    shaderProgram.SetFloat("pointLight[1].quadraticAttenuation", 0.032f);
    shaderProgram.SetVec3("pointLight[1].ambient", 0.2f, 0.2f, 0.2f);
    shaderProgram.SetVec3("pointLight[1].diffuse", 0.7f, 0.0f, 0.7f);
    shaderProgram.SetVec3("pointLight[1].specular", 1.0f, 1.0f, 1.0f);
    // Point Light 3
    shaderProgram.SetVec3("pointLight[2].position", pointLightPositions[2]);
    shaderProgram.SetFloat("pointLight[2].constantAttenuation",  1.0f);
    shaderProgram.SetFloat("pointLight[2].linearAttenuation",    0.09f);
    shaderProgram.SetFloat("pointLight[2].quadraticAttenuation", 0.032f);
    shaderProgram.SetVec3("pointLight[2].ambient", 0.2f, 0.2f, 0.2f);
    shaderProgram.SetVec3("pointLight[2].diffuse", 0.7f, 0.7f, 0.0f);
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
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    stbi_set_flip_vertically_on_load(true);                     // Backpack model has a pre-flipped texture, which needs flipping back to the flipped version, as the model-loader flips the model-UVs.
    Model backpack("../../resources/Backpack/backpack.obj");
    stbi_set_flip_vertically_on_load(false);                    // Swap back to normal flipping.
    Model grass("../../resources/Grass/grass.obj");
    Model windowPane("../../resources/Window/window.obj");

    // Create the drawable frame-buffer.
    unsigned int frameBuffer;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    // Create the colour component of the frame-buffer through a texture that can be read from the GPU.
    unsigned int textureColourBuffer;
    glGenTextures(1, &textureColourBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColourBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Attach the texture to the currently bound frame-buffer object.
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColourBuffer, 0);

    // Create the depth and stencil components of the frame-buffer using a render-buffer object, which cannot be read from the GPU (efficiently).
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Attach the render-buffer object to the currently bound frame-buffer object.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Check that the frame-buffer is complete.
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR:FRAMEBUFFER::NOT::COMPLETE" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Set up the screen-space quad to render the frameBuffer to the screen.
    unsigned int fbVBO, fbVAO;
    glGenVertexArrays(1, &fbVAO);
    glBindVertexArray(fbVAO);

    glGenBuffers(1, &fbVBO);
    glBindBuffer(GL_ARRAY_BUFFER, fbVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(frameBufferQuad), frameBufferQuad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Create Cube-Map texture.
    unsigned int cubeMapTexture = LoadCubeMap(cubeMapFaces);

    // Set up the cube to render the Cube-Map texture to.
    unsigned int cmVBO, cmVAO;

    glGenVertexArrays(1, &cmVAO);
    glBindVertexArray(cmVAO);

    glGenBuffers(1, &cmVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cmVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (running)
    {
        float currentFrame = (double)SDL_GetTicks() / 1000;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        running = ProcessInput(window, event, camera);

        // Bind off-screen frame-buffer to render to a texture.
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        viewMatrix = camera.GetViewMatrix();
        projectionMatrix = glm::perspective(glm::radians(camera.GetFOV()), 1920.0f / 1080.0f, 0.1f, 100.0f);
        shaderProgram.SetVec3("viewPos", camera.GetPosition());

        shaderProgram.Use();

        glDepthMask(GL_TRUE);
        unsigned int modelMatrixUniformLocation = glGetUniformLocation(shaderProgram.GetID(), "modelMatrix");
        unsigned int viewMatrixUniformLocation = glGetUniformLocation(shaderProgram.GetID(), "viewMatrix");
        unsigned int projectionMatrixUniformLocation = glGetUniformLocation(shaderProgram.GetID(), "projectionMatrix");
        glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(viewMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        backpack.Draw(shaderProgram);

        reflectiveShader.Use();

        modelMatrix = glm::mat4(1.0);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-5.0f, 0.0f, 0.0f));

        modelMatrixUniformLocation = glGetUniformLocation(reflectiveShader.GetID(), "modelMatrix");
        viewMatrixUniformLocation = glGetUniformLocation(reflectiveShader.GetID(), "viewMatrix");
        projectionMatrixUniformLocation = glGetUniformLocation(reflectiveShader.GetID(), "projectionMatrix");
        glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(viewMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        reflectiveShader.SetVec3("cameraPosition", camera.GetPosition());

        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);

        backpack.Draw(reflectiveShader);

        refractiveShader.Use();

        modelMatrix = glm::mat4(1.0);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(5.0f, 0.0f, 0.0f));

        modelMatrixUniformLocation = glGetUniformLocation(refractiveShader.GetID(), "modelMatrix");
        viewMatrixUniformLocation = glGetUniformLocation(refractiveShader.GetID(), "viewMatrix");
        projectionMatrixUniformLocation = glGetUniformLocation(refractiveShader.GetID(), "projectionMatrix");
        glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(viewMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        refractiveShader.SetVec3("cameraPosition", camera.GetPosition());

        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);

        backpack.Draw(refractiveShader);

        skyboxShader.Use();

        viewMatrix = glm::mat4(glm::mat3(camera.GetViewMatrix()));

        glDepthFunc(GL_LEQUAL);
        viewMatrixUniformLocation = glGetUniformLocation(skyboxShader.GetID(), "viewMatrix");
        projectionMatrixUniformLocation = glGetUniformLocation(skyboxShader.GetID(), "projectionMatrix");
        glUniformMatrix4fv(viewMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        glBindVertexArray(cmVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);

        blendingShader.Use();
        viewMatrix = camera.GetViewMatrix();

        modelMatrixUniformLocation = glGetUniformLocation(blendingShader.GetID(), "modelMatrix");
        viewMatrixUniformLocation = glGetUniformLocation(blendingShader.GetID(), "viewMatrix");
        projectionMatrixUniformLocation = glGetUniformLocation(blendingShader.GetID(), "projectionMatrix");
        glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(viewMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        std::map<float, glm::vec3> sorted;
        const unsigned int noPanes = sizeof(windowPanePositions) / sizeof(windowPanePositions[0]);
        for (unsigned int i = 0; i < noPanes; i++)
        {
            float distance = glm::length(camera.GetPosition() - windowPanePositions[i]);
            sorted[distance] = windowPanePositions[i];
        }

        for(std::map<float,glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it) 
        {
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, it->second);		
            modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));		
            glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
            windowPane.Draw(blendingShader);
        }  

        modelMatrix = glm::mat4(1.0f);

        // Render the texture to the screen-space frame-buffer quad to allow for post-processing.
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        framebufferShader.Use();
        glBindVertexArray(fbVAO);
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, textureColourBuffer);
        glUniform1i(glGetUniformLocation(framebufferShader.GetID(), "screenTexture"), 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

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

unsigned int LoadCubeMap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, noChannels;

    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &noChannels, 0);

        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}