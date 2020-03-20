#ifndef RENDEN_CONTROL_IMGUI_INTERFACE_H_
#define RENDEN_CONTROL_IMGUI_INTERFACE_H_

#include <GLFW/glfw3.h>

namespace control
{
	void imgui_init(GLFWwindow*);
	void imgui_frame(GLFWwindow*);
	void imgui_cleanup();
}

#endif
