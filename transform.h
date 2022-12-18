#ifndef TRANSFORM_DATASTRUCTURE_H
#define TRANSFORM_DATASTRUCTURE_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtc/quaternion.hpp"

struct Transform {
    glm::vec3 position;
    glm::vec3 scale = glm::vec3(1.0);
    glm::vec3 rotation;

    inline glm::vec3 Forward(){
        return glm::normalize(glm::quat(glm::vec3(glm::radians(rotation.y), 
                                                    glm::radians(-rotation.x), 
                                                    glm::radians(rotation.z))) * glm::vec3(0,1,0));

        glm::vec3 front;
        front.x = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
        front.y = sin(glm::radians(rotation.y));
        front.z = sin(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
        return glm::normalize(front);
    }

    inline glm::vec3 Right(){
        return glm::normalize(glm::cross(Forward(), glm::vec3(0, 1.0, 0)));
    }

    glm::vec3 Up(){
        return -glm::normalize(glm::cross(Forward(), Right()));
    }

    glm::mat4 GetModelMatrix(){
        glm::mat4 modelMat = glm::mat4(1.0);
        modelMat = glm::scale(modelMat, scale);
        modelMat = glm::translate(modelMat, position);
        modelMat = glm::rotate(modelMat, rotation.x, glm::vec3(1.0, 0.0, 0.0));
        modelMat = glm::rotate(modelMat, rotation.y, glm::vec3(0.0, 1.0, 0.0));
        modelMat = glm::rotate(modelMat, rotation.z, glm::vec3(0.0, 0.0, 1.0));
        return modelMat;
    }
};

#endif