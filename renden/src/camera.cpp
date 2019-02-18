#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"




camera::camera(GLFWwindow* window, float width, float height) : window(window), width(width), height(height)
{
}

camera::~camera()
{
}

void camera::update(float deltaTime, bool focus)
{
    // calculate mouse movement
    if (focus) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        glfwSetCursorPos(window, width / 2, height / 2);
        yaw += mouseSpeed * deltaTime * float(width / 2 - xpos);
        pitch += mouseSpeed * deltaTime * float(height / 2 - ypos);
        if (pitch > glm::pi<float>() / 2 - 0.01)
            pitch = static_cast<float>(glm::pi<float>() / 2 - 0.01);
        if (pitch < -glm::pi<float>() / 2 + 0.01)
            pitch = static_cast<float>(-glm::pi<float>() / 2 + 0.01);
    }

    // calculate camera vectors
    glm::vec3 direction = normalize(glm::vec3(
            std::cos(pitch) * std::sin(yaw),
            std::sin(pitch),
            std::cos(pitch) * std::cos(yaw)
    ));

    glm::vec3 right = normalize(cross(direction, up));

    // handle input
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position += direction * deltaTime * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position -= direction * deltaTime * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position += right * deltaTime * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position -= right * deltaTime * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        position -= glm::vec3(0, 1, 0) * deltaTime * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        position += glm::vec3(0, 1, 0) * deltaTime * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
        if (fov > 0.01)
            fov -= 0.01;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
        if (fov < glm::pi<float>() - 0.01)
            fov += 0.01;
    }

    view = glm::lookAt(
            position,
            position + direction,
            up
    );
    proj = glm::perspective(fov, width / height, 0.01f, 100.0f);
}