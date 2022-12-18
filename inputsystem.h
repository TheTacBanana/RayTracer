#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#include "glm/glm.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <unordered_map>
#include <string>

struct KeyPress{
    int key;
    int scancode;
    int action;
    int mods;
};

class InputSystem{
    public:
        std::unordered_map<int, KeyPress> pressedKeys;

        glm::vec2 lastMouseAxis;
        glm::vec2 mouseAxis;

        float mouseX;
        float mouseY;

        float lastX;
        float lastY;
        bool firstMouse = true;

        static InputSystem& GetInstance(){
            static InputSystem instance;
            return instance;
        }

        void SetKey(KeyPress kp){
            pressedKeys[kp.key] = kp;
        } 

        bool GetKey(int keyval){
            int action = pressedKeys[keyval].action;
            if (action == GLFW_PRESS || action == GLFW_REPEAT)
                return true;
            return false;
        } 

        glm::vec2 GetMouseAxis(){
            if (firstMouse)
            {
                lastX = mouseAxis.x;
                lastY = mouseAxis.y;
                firstMouse = false;
            }

            mouseX = mouseAxis.x - lastX;
            mouseY = lastY - mouseAxis.y;

            lastX = mouseAxis.x;
            lastY = mouseAxis.y;

            return glm::vec2(mouseX, mouseY);
        }

        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
            GetInstance().key_callback_impl(window, key, scancode, action, mods);
        }

        void key_callback_impl(GLFWwindow* window, int key, int scancode, int action, int mods){
            SetKey({key, scancode, action, mods});
        }

        static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn){
            GetInstance().mouse_callback_impl(window, xposIn, yposIn);
        }

        void mouse_callback_impl(GLFWwindow* window, double xposIn, double yposIn){
            mouseAxis = glm::vec2(xposIn, yposIn);
        }


    private:
        InputSystem(void) // private constructor necessary to allow only 1 instance
        {
        }

        InputSystem(InputSystem const&); // prevent copies
        void operator=(InputSystem const&);


};   

#endif