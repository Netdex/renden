// Local Headers


#include "glitter.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <gl/shader.hpp>
#include <chrono>
#include <memory>

#include <gl/buffer.hpp>
#include <gl/varray.hpp>
#include <camera.hpp>
#include <gl/mesh.hpp>
#include <gl/texture.hpp>
#include <loader/shader_manager.hpp>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <primitive/block_primitive.hpp>
#include <loader/block_manager.hpp>
#include <world/chunk.hpp>
#include <world/block.hpp>


bool wireframe = false;
bool focus = true;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_E:
                wireframe = !wireframe;
                if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                else
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;
            case GLFW_KEY_LEFT_ALT:
                focus = !focus;
                if (focus) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                break;
        }
    }
}

void APIENTRY debug_callback(GLenum source, GLenum type, GLuint id,
                             GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
    printf("[");
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            printf("High");
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            printf("Medium");
            break;
        case GL_DEBUG_SEVERITY_LOW:
            printf("Low");
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            printf("Notification");
            break;
    }
    printf("] ");

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            printf("Error");
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            printf("Deprecated Behaviour");
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            printf("Undefined Behaviour");
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            printf("Portability");
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            printf("Performance");
            break;
        case GL_DEBUG_TYPE_MARKER:
            printf("Marker");
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            printf("Push Group");
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            printf("Pop Group");
            break;
        case GL_DEBUG_TYPE_OTHER:
            printf("Other");
            break;
    }
    printf(" - ");
    switch (source) {
        case GL_DEBUG_SOURCE_API:
            printf("API");
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            printf("Window System");
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            printf("Shader Compiler");
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            printf("Third Party");
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            printf("Application");
            break;
        case GL_DEBUG_SOURCE_OTHER:
            printf("Other");
            break;
    }
    printf(": %s\n", message);
}

int main(int argc, char *argv[]) {
    // Load GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    auto mWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL", nullptr, nullptr);

    // Check for Valid Context
    if (mWindow == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Context");
        return EXIT_FAILURE;
    }

    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(mWindow);
    glfwSwapInterval(1);
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetKeyCallback(mWindow, key_callback);

    gladLoadGL();
    fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(debug_callback, nullptr);

    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    camera cam(mWindow, WINDOW_WIDTH, WINDOW_HEIGHT);

    {
        auto all_shaders = shaders::load();
        auto all_blocks = world::entities::blocks::load();

        // temporary for testing
        chunk cnk;

        for (int i = 0; i < 5; i++)
            cnk.data[i + 3][8][8] = block(17);
        for (int y = 0; y < 3; y++)
            for (int z = 0; z < 16; z++)
                for (int x = 0; x < 16; x++) {
                    if (y == 2)
                        cnk.data[y][z][x] = block(2);
                    else
                        cnk.data[y][z][x] = block(3);
                }

        for (int y = 8; y < 13; y++) {
            for (int z = 6; z < 11; z++)
                for (int x = 6; x < 11; x++) {
                    cnk.data[y][z][x] = block(18);
                }
        }
        cnk.data[10][10][10] = block(5);
        cnk.data[10][10][9] = block(4);
        cnk.data[10][10][8] = block(40);

        cnk.update_mesh();


        float lastTick = (float) glfwGetTime();
        // Rendering Loop
        while (glfwWindowShouldClose(mWindow) == false) {
            if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(mWindow, true);

            float now = (float) glfwGetTime();
            float deltaTime = now - lastTick;
            lastTick = now;
            cam.update(deltaTime, focus);


            // Background Fill Color
            glClearColor(1.f, 1.f, 1.f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


            if (auto block = shaders::block::shader.lock()) {
                block->activate();
                block->bind("view", cam.view);
                block->bind("proj", cam.proj);

                cnk.draw(*block, glm::mat4(1.f));
            }

            if (auto tenbox = shaders::tenbox::shader.lock()) {
                glDepthFunc(GL_LEQUAL);
                tenbox->activate();
                tenbox->bind("view", glm::mat4(glm::mat3(cam.view))); // remove translation
                tenbox->bind("proj", cam.proj);
                shaders::tenbox::tenbox->draw(*tenbox);
                glDepthFunc(GL_LESS);
            }


//        for (int z = 0; z < 1; z++) {
//            for (int x = 0; x < 1; x++) {
//glm::translate(glm::mat4(1.f), glm::vec3(x, sin((x * z) / 100.f + now) * 10, z))

//        }

            // Flip Buffers and Draw
            glfwSwapBuffers(mWindow);
            glfwPollEvents();
        }
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}
