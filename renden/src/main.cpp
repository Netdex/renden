#include "glitter.hpp"

#include <cstdlib>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION

#include "camera.hpp"
#include "debug.hpp"
#include "gl/shader.hpp"
#include "loader/block_manager.hpp"
#include "loader/chunk_manager.hpp"
#include "loader/shader_manager.hpp"
#include "phy/phy_engine.hpp"
#include "primitive/block_primitive.hpp"
#include "world/block.hpp"
#include "world/chunk.hpp"
#include "world/reticle.hpp"

bool wireframe = false;
bool focus = true;

int active_block = 1;

std::optional<std::pair<glm::ivec3, glm::ivec3>> target;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_E:
			wireframe = !wireframe;
			if (wireframe)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case GLFW_KEY_LEFT_ALT:
			focus = !focus;
			if (focus) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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
			active_block = key - GLFW_KEY_0;
			break;
		default: break;
		}
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	auto cnk_mgr = world::chunk::db.lock();

	if (action == GLFW_PRESS)
	{
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			if (target)
				cnk_mgr->GetBlockRefAt(target->first)->get() = Block();
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			if (target)
				cnk_mgr->GetBlockRefAt(target->first + target->second, true)->get() = Block(active_block);
			break;
		default: break;
		}
	}
}

int main(int argc, char* argv[])
{
	spdlog::set_level(spdlog::level::trace);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	const auto m_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "renden", nullptr, nullptr);
	glfwSetMouseButtonCallback(m_window, mouse_button_callback);

	if (m_window == nullptr)
	{
		spdlog::critical("Failed to create OpenGL context!");
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1);
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	glfwSetKeyCallback(m_window, key_callback);

	gladLoadGL();
	spdlog::info("OpenGL {}", glGetString(GL_VERSION));

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(debug_callback, nullptr);

	glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//	glLineWidth(10.f);

	Camera cam(m_window, WINDOW_WIDTH, WINDOW_HEIGHT);

	{
		auto all_shaders = shaders::Load();
		auto all_blocks = world::block::load();
		auto all_chunks = world::chunk::load();

		auto chunks = world::chunk::db.lock();
		auto block_shader = shaders::block::shader.lock();
		auto tenbox_shader = shaders::tenbox::shader.lock();
		auto reticle_shader = shaders::reticle::shader.lock();

		//phy_engine phy;

		Reticle reticle;

		const int scz = 8;
		const int scx = 8;

		for (int cz = 0; cz < scz; cz++)
		{
			for (int cx = 0; cx < scx; cx++)
			{
				auto cnk = chunks->GetChunkAt(cx - 4, cz - 4);
				for (int i = 0; i < 5; i++)
					cnk->GetBlockRefAt(8, i + 3 + 64, 8)->get() = Block(17);
				for (int y = 0; y < 3 + 64; y++)
					for (int z = 0; z < 16; z++)
						for (int x = 0; x < 16; x++)
						{
							if (y == 2 + 64)
								cnk->GetBlockRefAt(x, y, z)->get() = Block(2);
							else
								cnk->GetBlockRefAt(x, y, z)->get() = Block(3);
						}

				for (int y = 8 + 64; y < 13 + 64; y++)
				{
					for (int z = 6; z < 11; z++)
						for (int x = 6; x < 11; x++)
						{
							cnk->GetBlockRefAt(x, y, z)->get() = Block(18);
						}
				}
				cnk->GetBlockRefAt(10, 10 + 64, 10)->get() = Block(5);
				cnk->GetBlockRefAt(10, 10 + 64, 9)->get() = Block(4);
				cnk->GetBlockRefAt(10, 10 + 64, 8)->get() = Block(40);
			}
		}

		auto last_tick = float(glfwGetTime());
		while (!static_cast<bool>(glfwWindowShouldClose(m_window)))
		{
			if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(m_window, true);

			const auto now = float(glfwGetTime());
			const float delta_time = now - last_tick;
			last_tick = now;
			cam.Update(delta_time, focus);

			target = cam.CastTarget(*chunks, 20);
			chunks->Update();

			glClearColor(1.f, 1.f, 1.f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			block_shader->Activate();
			//			block_shader->bind("now", now);
			block_shader->Bind("view", cam.View);
			block_shader->Bind("proj", cam.Proj);
			//			block_shader->bind("camera_pos", cam.position);
			chunks->Render(*block_shader);

			reticle_shader->Activate();
			reticle.Draw(*reticle_shader, cam.View, cam.Proj,
			             cam.Position, cam.GetDirection(),
			             target ? std::optional<glm::ivec3>(target->first) : std::nullopt);


			glDepthFunc(GL_LEQUAL);
			tenbox_shader->Activate();
			// Remove translation from view matrix by truncation.
			tenbox_shader->Bind("view", glm::mat4(glm::mat3(cam.View)));
			tenbox_shader->Bind("proj", cam.Proj);
			shaders::tenbox::tenbox->Draw(*tenbox_shader);
			glDepthFunc(GL_LESS);

			glfwSwapBuffers(m_window);
			glfwPollEvents();
		}
	}
	glfwTerminate();
	return EXIT_SUCCESS;
}
