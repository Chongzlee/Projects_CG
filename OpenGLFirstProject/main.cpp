
#undef GLFW_DLL
#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <cmath>

#include "Libs/Shader.h"
#include "Libs/Window.h"
#include "Libs/Mesh.h"
#include "Libs/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const GLint WIDTH = 800, HEIGHT = 600;

float pitch = 0.0f, yaw = -90.f;

Window mainWindow;
std::vector<Mesh *> meshList;
std::vector<Shader *> shaderList;

// Vertex Shader
static const char *vShader = "Shaders/shader.vert";

// Fragment Shader
static const char *fShader = "Shaders/shader.frag";
Mesh *light;

// Vertex Shader
static const char *lightvShader = "Shaders/lightshader.vert";

// Fragment Shader
static const char *lightfShader = "Shaders/lightshader.frag";

Mesh *robo;
// Vertex Shader
static const char *robovShader = "Shaders/roboshader.vert";

// Fragment Shader
static const char *robofShader = "Shaders/roboshader.frag";

Mesh *gun;
// Vertex Shader
static const char *gunvShader = "Shaders/gunshader.vert";

// Fragment Shader
static const char *gunfShader = "Shaders/gunshader.frag";

void CreateTriangle()
{
    GLfloat vertices[] =
        {
            // pos                         //aTexCoord
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, -1.0f, 1.0f, 0.5f, 0.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.5f, 1.0f};

    unsigned int indices[] =
        {
            0,
            3,
            1,
            1,
            3,
            2,
            2,
            3,
            0,
            0,
            1,
            2,
        };

    Mesh *obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, 20, 12);

    meshList.push_back(obj1);
}

void CreateShaders()
{
    Shader *shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(shader1);

    Shader *shader2 = new Shader();
    shader2->CreateFromFiles(lightvShader, lightfShader);
    shaderList.push_back(shader2);

    Shader *shader3 = new Shader();
    shader3->CreateFromFiles(robovShader, robofShader);
    shaderList.push_back(shader3);

    Shader *shader4 = new Shader();
    shader4->CreateFromFiles(gunvShader, gunfShader);
    shaderList.push_back(shader4);
}

void checkMouse()
{
    float xoffset = 0, yoffset = 0;

    double xpos, ypos;
    glfwGetCursorPos(mainWindow.getWindow(), &xpos, &ypos);

    static float lastX = xpos;
    static float lastY = ypos;

    xoffset = xpos - lastX;
    yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89)
        pitch = 89;
    if (pitch < -89)
        pitch = -89;
}

void CreateOBJ()
{
    Mesh *obj1 = new Mesh();
    bool loaded = obj1->CreateMeshFromOBJ("Models/land02.obj");

    if (loaded)
    {
        for (int i = 0; i < 10; i++)
        {
            meshList.push_back(obj1);
        }
    }
    else
    {
        std::cout << "Failed to load model" << std::endl;
    }

    light = new Mesh();
    loaded = light->CreateMeshFromOBJ("Models/ufo.obj");
    if (!loaded)
    {
        std::cout << "Failed to load model" << std::endl;
    }

    robo = new Mesh();
    loaded = robo->CreateMeshFromOBJ("Models/robotic.obj");
    if (!loaded)
    {
        std::cout << "Failed to load model" << std::endl;
    }

    gun = new Mesh();
    loaded = gun->CreateMeshFromOBJ("Models/gun.obj");
    if (!loaded)
    {
        std::cout << "Failed to load model" << std::endl;
    }
}

// change light to cyan
glm::vec3 lightColour = glm::vec3(1.0f, 1.0f, 1.0f);

int main()
{
    mainWindow = Window(WIDTH, HEIGHT, 3, 3); // mac 4,1 , no gpu 3,1 (Major Minor)
    mainWindow.initialise();

    // CreateTriangle();
    CreateOBJ();
    CreateShaders();

    GLuint uniformModel = 0, uniformProjection = 0, uniformView = 0;

    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight(), 0.1f, 300.0f);
    // glm::mat4 projection = glm::ortho(-0.4f, 0.4f, -0.3f, 0.3f, 0.1f, 100.0f);
    // glm::mat4 projection = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.1f, 100.0f);

    glm::vec3 cameraPos = glm::vec3(0, 0, 10);
    glm::vec3 cameraTarget = glm::vec3(0, 0, -1);
    glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPos);

    glm::vec3 up = glm::vec3(0, 1, 0);
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraDirection, up));
    glm::vec3 cameraUp = glm::normalize(glm::cross(cameraRight, cameraDirection));

    float deltaTime, lastFrame = 0;

    int width, height, nrChannels;
    unsigned char *data = stbi_load("Textures/05.png", &width, &height, &nrChannels, 0);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load Texture" << std::endl;
    }
    stbi_image_free(data);

    int width2, height2, nrChannels2;
    unsigned char *data2 = stbi_load("Textures/03.png", &width2, &height2, &nrChannels2, 0);

    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (data2)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width2, height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load Texture" << std::endl;
    }
    stbi_image_free(data2);

    int width4, height4, nrChannels4;
    unsigned char *data4 = stbi_load("Textures/uvmap.png", &width4, &height4, &nrChannels4, 0);

    unsigned int texture4;
    glGenTextures(1, &texture4);
    glBindTexture(GL_TEXTURE_2D, texture4);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (data4)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width4, height4, 0, GL_RGBA, GL_UNSIGNED_BYTE, data4);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load Texture" << std::endl;
    }
    stbi_image_free(data4);

    // Loop until window closed
    while (!mainWindow.getShouldClose())
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // Get + Handle user input events
        glfwPollEvents();

        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_W) == GLFW_PRESS)
        {
            cameraPos += cameraDirection * deltaTime * 8.0f;
        }
        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_S) == GLFW_PRESS)
        {
            cameraPos -= cameraDirection * deltaTime * 8.0f;
        }
        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_A) == GLFW_PRESS)
        {
            cameraPos -= cameraRight * deltaTime * 8.0f;
        }
        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_D) == GLFW_PRESS)
        {
            cameraPos += cameraRight * deltaTime * 8.0f;
        }
        checkMouse();

        glm::vec3 direction;
        direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
        direction.y = sin(glm::radians(pitch));
        direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

        cameraDirection = glm::normalize(direction);
        cameraRight = glm::normalize(glm::cross(cameraDirection, up));
        cameraUp = glm::normalize(glm::cross(cameraRight, cameraDirection));

        // glm ::vec3 lightPos = glm::vec3(-100, 0, 0);
        // lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
        // lightPos.y = sin(glfwGetTime() / 2.0f) * 2.0f;

        glm::vec3 lightPos = glm::vec3(-100, 0, 0);
        float radius = 15.0f;
        float speed = 1.0f;

        lightPos.x = radius * cos(glfwGetTime() * speed);
        lightPos.z = radius * sin(glfwGetTime() * speed);

        // Clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // cloour
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw here
        shaderList[0]->UseShader();
        uniformModel = shaderList[0]->GetUniformLocation("model");
        uniformProjection = shaderList[0]->GetUniformLocation("projection");
        uniformView = shaderList[0]->GetUniformLocation("view");

        glm::vec3 pyramidPositions[] =
            {
                glm::vec3(0.0f, 0.0f, -2.5f),
                glm::vec3(2.0f, 5.0f, -15.0f),
                glm::vec3(-1.5f, -2.2f, -2.5f),
                glm::vec3(-3.8f, -2.0f, -12.3f),
                glm::vec3(2.4f, -0.4f, -3.5f),
                glm::vec3(-1.7f, 3.0f, -7.5f),
                glm::vec3(1.3f, -2.0f, -2.5f),
                glm::vec3(1.5f, 2.0f, -2.5f),
                glm::vec3(1.5f, 0.2f, -1.5f),
                glm::vec3(-1.3f, 1.0f, -1.5f)};

        glm::mat4 view(1.0f);

        glm::mat4 cameraPosMat(1.0f);

        cameraPosMat[3][0] = -cameraPos.x;
        cameraPosMat[3][1] = -cameraPos.y;
        cameraPosMat[3][2] = -cameraPos.z;

        glm::mat4 cameraRotateMat(1.0f);
        cameraRotateMat[0] = glm::vec4(cameraRight.x, cameraUp.x, -cameraDirection.x, 0);
        cameraRotateMat[1] = glm::vec4(cameraRight.y, cameraUp.y, -cameraDirection.y, 0);
        cameraRotateMat[2] = glm::vec4(cameraRight.z, cameraUp.z, -cameraDirection.z, 0);

        view = cameraRotateMat * cameraPosMat;
        // robo
        glm::mat4 model_robotic(1.0f);

        model_robotic = glm::translate(model_robotic, pyramidPositions[0]);
        float angle = 20.0f * 1;
        // model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model_robotic = glm::translate(model_robotic, glm::vec3(10.0f, -20.0f, -20.0f));
        // model = glm::rotate(model, (float)glfwGetTime() * glm::radians(90.0f), glm::vec3(1.0f, 1.0f, 0.0f));
        model_robotic = glm::scale(model_robotic, glm::vec3(1.0f, 1.0f, 1.0f));

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_robotic));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));

        // light
        glUniform3fv(shaderList[2]->GetUniformLocation("lightColour"), 1, (GLfloat *)&lightColour);
        glUniform3fv(shaderList[2]->GetUniformLocation("lightPos"), 1, (GLfloat *)&lightPos);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture2);

        robo->RenderMesh();
        ;

        // gun
        shaderList[3]->UseShader();
        uniformModel = shaderList[3]->GetUniformLocation("model");
        uniformProjection = shaderList[3]->GetUniformLocation("projection");
        uniformView = shaderList[3]->GetUniformLocation("view");

        glm::mat4 model_gun(1.0f);

        model_gun = glm::translate(model_gun, glm::vec3(10.0f, -20.0f, -20.0f));

        model_gun = glm::scale(model_gun, glm::vec3(1.0f, 1.0f, 1.0f));

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_gun));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));

        glUniform3fv(shaderList[3]->GetUniformLocation("lightColour"), 1, (GLfloat *)&lightColour);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture4);
        gun->RenderMesh();

        glm::mat4 model(1.0f);

        model = glm::translate(model, pyramidPositions[0]);
        float angle_land = 20.0f * 1;
        // model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, glm::vec3(-30.0f, -30.0f, -30.0f));
        // model = glm::rotate(model, (float)glfwGetTime() * glm::radians(90.0f), glm::vec3(1.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.8f, 0.8f, 1.0f));

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));

        
        glUniform3fv(shaderList[0]->GetUniformLocation("lightColour"), 1, (GLfloat *)&lightColour);
        glUniform3fv(shaderList[0]->GetUniformLocation("lightPos"), 1, (GLfloat *)&lightPos);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        meshList[0]->RenderMesh();

        // light
        shaderList[1]->UseShader();
        uniformModel = shaderList[1]->GetUniformLocation("model");
        uniformProjection = shaderList[1]->GetUniformLocation("projection");
        uniformView = shaderList[1]->GetUniformLocation("view");

        glm::mat4 model_light(1.0f);

        model_light = glm::translate(model_light, lightPos);
        model_light = glm::translate(model_light, glm::vec3(-10.0f, 5.0f, 0.0f));
        model_light = glm::scale(model_light, glm::vec3(0.5f, 0.5f, 0.5f));

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model_light));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));

        glUniform3fv(shaderList[1]->GetUniformLocation("lightColour"), 1, (GLfloat *)&lightColour);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture2);
        light->RenderMesh();

        glUseProgram(0);
        // end draw

        mainWindow.swapBuffers();
    }

    return 0;
}
