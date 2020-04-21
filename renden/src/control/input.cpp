#include "control/input.hpp"

#include <glad/glad.h>
#include <imgui.h>

#include "world/world.hpp"

namespace control
{
ControllerState state;

void key_callback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/)
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureKeyboard)
		return;
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_E:
			state.wireframe = !state.wireframe;
			glPolygonMode(GL_FRONT_AND_BACK, state.wireframe ? GL_LINE : GL_FILL);
			break;
		case GLFW_KEY_LEFT_ALT:
			state.focus = !state.focus;
			break;
		case GLFW_KEY_1:
		case GLFW_KEY_2:
		case GLFW_KEY_3:
		case GLFW_KEY_4:
		case GLFW_KEY_5:
		case GLFW_KEY_6:
		case GLFW_KEY_7:
		case GLFW_KEY_8:
		case GLFW_KEY_9:
			state.active_block = block_id_t(key - GLFW_KEY_0);
			break;
		default: break;
		}
	}
}

void mouse_button_callback(GLFWwindow* /*window*/, int button, int action, int /*mods*/)
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse)
		return;
	world::World& world = Context<world::World>::Get();
	if (action == GLFW_PRESS)
	{
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			if (state.target)
				*world.GetBlockRefAt(state.target->first) = Block();
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			if (state.target)
				*world.GetBlockRefAt(state.target->first + state.target->second, true) =
					Block(state.active_block);
			break;
		default: break;
		}
	}
}
}
