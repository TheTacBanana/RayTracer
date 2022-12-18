#include <iostream>
#include <experimental/filesystem>

#include "resourceloader.h"
#include "inputsystem.h"
#include "shader.h"
#include "transform.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLFWwindow* window;
unsigned int SCR_WIDTH = 1200;
unsigned int SCR_HEIGHT = 1200;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Framebuffer Callback
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Key & Mouse Callback
    glfwSetKeyCallback(window, InputSystem::GetInstance().key_callback);
    glfwSetCursorPosCallback(window, InputSystem::GetInstance().mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glPolygonMode( GL_FRONT, GL_TRIANGLES );

    ResourceLoader::SetResourcePath("Resources/");
    ResourceLoader::LoadResource<Shader>("Everything", "everything.vs", "raytracer.fs");
    Shader* shader = ResourceLoader::GetResource<Shader>("Everything");

     float vertices[] = {
         1.0f,  1.0f, 0.0f, 
         1.0f, -1.0f, 0.0f, 
        -1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f  
    };
    unsigned int indices[] = {  
        0, 1, 3,
        1, 2, 3 
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    glBindVertexArray(0);

    Transform* trans = new Transform();

    InputSystem& input = InputSystem::GetInstance();
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (input.GetKey(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, true);

        float camSpeed = 0;
        if (input.GetKey(GLFW_KEY_LEFT_SHIFT)) camSpeed = deltaTime * float(5);
        else camSpeed = deltaTime * float(2);

        if (input.GetKey('W')) trans->position -= glm::vec3(0,0,1) * camSpeed;
        if (input.GetKey('S')) trans->position += glm::vec3(0,0,1) * camSpeed;
        if (input.GetKey('A')) trans->position -= glm::vec3(1,0,0) * camSpeed;
        if (input.GetKey('D')) trans->position += glm::vec3(1,0,0) * camSpeed;
        if (input.GetKey(GLFW_KEY_SPACE)) trans->position += glm::vec3(0,1,0) * camSpeed;
        if (input.GetKey(GLFW_KEY_LEFT_CONTROL)) trans->position -= glm::vec3(0,1,0) * camSpeed;
        trans->rotation += glm::vec3(input.GetMouseAxis() * 0.1f, 0);

        shader->use();
        shader->setVec3("camPos", trans->position);
        shader->setVec2("viewportDimensions", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
        shader->setFloat("imageAspectRatio", float(SCR_WIDTH) / float(SCR_HEIGHT));
        glBindVertexArray(VAO); 
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}