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
#include "spdlog/spdlog.h"

#include <debug.hpp>
#include <primitive/block_primitive.hpp>
#include <loader/block_manager.hpp>
#include <world/chunk.hpp>
#include <world/block.hpp>
#include <loader/chunk_manager.hpp>
#include "world/reticle.hpp"

bool wireframe = false;
bool focus = true;

std::optional<std::pair<glm::ivec3, glm::ivec3>> target;

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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	auto cnk_mgr = world::chunk::db.lock();

	if (action == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT:
			if (target)
				cnk_mgr->get_block_ref_at(target->first)->id = 0;
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			if (target)
				cnk_mgr->get_block_ref_at(target->first + target->second, true)->id = 1;
			break;
		}
	}
}

int main(int argc, char *argv[]) {
	spdlog::set_level(spdlog::level::debug);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	auto mWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "renden", nullptr, nullptr);
	glfwSetMouseButtonCallback(mWindow, mouse_button_callback);

	if (mWindow == nullptr) {
		fprintf(stderr, "Failed to Create OpenGL Context");
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(mWindow);
	glfwSwapInterval(1);
	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	glfwSetKeyCallback(mWindow, key_callback);

	gladLoadGL();
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

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

	camera cam(mWindow, WINDOW_WIDTH, WINDOW_HEIGHT);

	{
		auto all_shaders = shaders::load();
		auto all_blocks = world::entities::blocks::load();
		auto all_chunks = world::chunk::load();

		auto chunks = world::chunk::db.lock();
		auto block_shader = shaders::block::shader.lock();
		auto tenbox_shader = shaders::tenbox::shader.lock();
		auto reticle_shader = shaders::reticle::shader.lock();

		reticle reticle;

		const int scz = 8;
		const int scx = 8;

		for (int cz = 0; cz < scz; cz++) {
			for (int cx = 0; cx < scx; cx++) {
				auto cnk = chunks->get_chunk_at(cx-4, cz-4);
				for (int i = 0; i < 5; i++)
					*cnk->get_block_ref_at(8, i + 3 + 64, 8) = block(17);
				for (int y = 0; y < 3 + 64; y++)
					for (int z = 0; z < 16; z++)
						for (int x = 0; x < 16; x++) {
							if (y == 2 + 64)
								*cnk->get_block_ref_at(z, y, x) = block(2);
							else
								*cnk->get_block_ref_at(z, y, x) = block(3);
						}

				for (int y = 8 + 64; y < 13 + 64; y++) {
					for (int z = 6; z < 11; z++)
						for (int x = 6; x < 11; x++) {
							*cnk->get_block_ref_at(z, y, x) = block(18);
						}
				}
				*cnk->get_block_ref_at(10, 10 + 64, 10) = block(5);
				*cnk->get_block_ref_at(10, 10 + 64, 9) = block(4);
				*cnk->get_block_ref_at(10, 10 + 64, 8) = block(40);
			}
		}

		float lastTick = (float)glfwGetTime();
		while (glfwWindowShouldClose(mWindow) == false) {
			if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(mWindow, true);

			float now = (float)glfwGetTime();
			float deltaTime = now - lastTick;
			lastTick = now;
			cam.update(deltaTime, focus);

			target = cam.cast_target(*chunks, 20);
			chunks->update_all_meshes();

			glClearColor(1.f, 1.f, 1.f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			block_shader->activate();
//			block_shader->bind("now", now);
			block_shader->bind("view", cam.view);
			block_shader->bind("proj", cam.proj);
//			block_shader->bind("camera_pos", cam.position);
			chunks->render(*block_shader);

			reticle_shader->activate();
			reticle.draw(*reticle_shader, cam.view, cam.proj,
				cam.position, cam.get_direction(),
				target ? std::optional<glm::ivec3>(target->first) : std::nullopt);


			glDepthFunc(GL_LEQUAL);
			tenbox_shader->activate();
			tenbox_shader->bind("view", glm::mat4(glm::mat3(cam.view))); // remove translation
			tenbox_shader->bind("proj", cam.proj);
			shaders::tenbox::tenbox->draw(*tenbox_shader);
			glDepthFunc(GL_LESS);

			glfwSwapBuffers(mWindow);
			glfwPollEvents();
		}
	}
	glfwTerminate();
	return EXIT_SUCCESS;
}
