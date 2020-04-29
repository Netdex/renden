#ifndef RENDEN_CONTROL_INPUT_H_
#define RENDEN_CONTROL_INPUT_H_

#include <optional>

#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

#include "world/block.hpp"

namespace control
{
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

struct ControllerState
{
	bool wireframe = false;
	bool focus = true;
	world::block_id_t active_block = 1;
	std::optional<std::pair<glm::ivec3, glm::ivec3>> target;
};
extern ControllerState state;

}

#endif
