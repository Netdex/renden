#include "glitter.hpp"

#include <cstdlib>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <imgui.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION

#include "control/camera.hpp"
#include "control/imgui_interface.hpp"
#include "control/input.hpp"
#include "gl/debug.hpp"
#include "gl/depthmap.hpp"
#include "gl/shader.hpp"
#include "loader/block_manager.hpp"
#include "world/world.hpp"
#include "loader/shader_program.hpp"
#include "phy/phy_engine.hpp"
#include "util/context.hpp"
#include "world/block.hpp"
#include "world/chunk.hpp"
#include "world/reticle.hpp"
#include "util/config.hpp"

//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//	glViewport(0, 0, width, height);
//}

void init(GLFWwindow* m_window)
{
	util::config::Load();

	auto& config = util::config::Get();
	int width, height, xpos, ypos;
	glfwGetWindowSize(m_window, &width, &height);
	glfwGetWindowPos(m_window, &xpos, &ypos);
	if (const auto& window_rect = config["window_rect"])
	{
		xpos = int(window_rect["x"].value_or(xpos));
		ypos = int(window_rect["y"].value_or(ypos));
		width = int(window_rect["w"].value_or(width));
		height = int(window_rect["h"].value_or(height));
		glfwSetWindowPos(m_window, xpos, ypos);
		glfwSetWindowSize(m_window, width, height);
	}

	Context<shader::BlockShader>::Initialize().GetShader().Activate();
	Context<world::BlockManager>::Initialize(PROJECT_SOURCE_DIR "/renden/res/block_texture.json",
	                                         PROJECT_SOURCE_DIR "/renden/res/block_def.json");
	Context<shader::BlockDepthShader>::Initialize().GetShader().Activate();
	Context<world::World>::Initialize();

	Context<shader::SkyboxShader>::Initialize().GetShader().Activate();
	const std::string cubemap_paths[6] = {
		PROJECT_SOURCE_DIR "/renden/res/skybox/alps_rt.tga",
		PROJECT_SOURCE_DIR "/renden/res/skybox/alps_lf.tga",
		PROJECT_SOURCE_DIR "/renden/res/skybox/alps_up.tga",
		PROJECT_SOURCE_DIR "/renden/res/skybox/alps_dn.tga",
		PROJECT_SOURCE_DIR "/renden/res/skybox/alps_bk.tga",
		PROJECT_SOURCE_DIR "/renden/res/skybox/alps_ft.tga",
	};
	Context<world::Skybox>::Initialize(cubemap_paths);

	Context<shader::ReticleShader>::Initialize().GetShader().Activate();
	Context<world::Reticle>::Initialize();

	Context<control::Camera>::Initialize(m_window);

	// Some silly code to generate test scenery.
	world::World& world = Context<world::World>::Get();

	int bx = 4;
	int by = 1;
	int bz = 4;

	for (int ax = -bx; ax < bx; ++ax)
	{
		for (int ay = -by; ay < by; ++ay)
		{
			for (int az = -bz; az < bz; ++az)
			{
				world::Chunk* chunk = world.GetChunkAt({ax, ay, az}, true);
				for (int cx = 0; cx < world::Chunk::kChunkWidth; cx++)
				{
					for (int cy = 0; cy < world::Chunk::kChunkWidth; cy++)
					{
						for (int cz = 0; cz < world::Chunk::kChunkWidth; cz++)
						{
							auto p = world::Chunk::chunk_to_block_pos({ax, ay, az}, {cx, cy, cz});
							Block& b = chunk->GetBlockRefAt({cx, cy, cz}, true);
							if (abs(100 * sin(0.001f * (p.x * p.x + p.z * p.z)) / 10.f - p.y) <= 1)
								b = Block(1);
						}
					}
				}
			}
		}
	}
}

void cleanup(GLFWwindow* m_window)
{
	auto& config = util::config::Get();
	int width, height, xpos, ypos;
	glfwGetWindowPos(m_window, &xpos, &ypos);
	glfwGetWindowSize(m_window, &width, &height);
	config.insert_or_assign("window_rect", toml::table{
		                        {
			                        {"x", xpos},
			                        {"y", ypos},
			                        {"w", width},
			                        {"h", height},
		                        }
	                        });

	util::config::Save();

	// Free all resources which depend on OpenGL context.
	Context<shader::BlockShader>::Reset();
	Context<world::BlockManager>::Reset();
	Context<world::World>::Reset();

	Context<shader::BlockDepthShader>::Reset();

	Context<shader::SkyboxShader>::Reset();
	Context<world::Skybox>::Reset();

	Context<shader::ReticleShader>::Reset();
	Context<world::Reticle>::Reset();

	Context<control::Camera>::Reset();
}

void loop(GLFWwindow* m_window)
{
	auto& block_shader = Context<shader::BlockShader>::Get().GetShader();
	auto& block_depth_shader = Context<shader::BlockDepthShader>::Get().GetShader();
	auto& tenbox_shader = Context<shader::SkyboxShader>::Get().GetShader();
	auto& reticle_shader = Context<shader::ReticleShader>::Get().GetShader();

	auto& block_manager = Context<world::BlockManager>::Get();
	//phy_engine phy;

	control::Camera& cam = Context<control::Camera>::Get();

	// TODO There's no need to store the world in context, purely lazy.
	world::World& world = Context<world::World>::Get();

	// TODO Tidy this code up.
	constexpr int SHADOW_WIDTH = 1024;

	const glm::vec3 light_dir = glm::normalize(glm::vec3{0, -1, 0});
	const float part_intervals[] = {0.f, 0.1f, 0.2f, 1.f};
	const gl::DepthMap shadowmap(SHADOW_WIDTH, shader::BlockDepthShader::kShadowmapTextureUnit, part_intervals);

	auto last_tick = float(glfwGetTime());
	while (!static_cast<bool>(glfwWindowShouldClose(m_window)))
	{
		control::imgui_frame_begin();

		if (glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS)
			glfwSetWindowShouldClose(m_window, true);

		// Pre-drawing computations.
		const auto now = float(glfwGetTime());
		const float delta_time = now - last_tick;
		last_tick = now;
		int fb_width, fb_height;
		glfwGetFramebufferSize(m_window, &fb_width, &fb_height);
		cam.Update(delta_time, control::state.focus);

		control::state.target = cam.CastTarget(world, 20);
		world.Update();

		shadowmap.Render(cam, block_shader, block_depth_shader, light_dir,
		                 [&world, &block_depth_shader, &cam] { world.Render(block_depth_shader, cam); });

		// Drawing begins!
		glViewport(0, 0, fb_width, fb_height);
		glClearColor(1.f, 1.f, 1.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		glCullFace(GL_BACK);
		glDisable(GL_DEPTH_CLAMP);
		glEnable(GL_CULL_FACE);

		// TODO Would like to do something similar to Skybox (i.e. WorldRenderer).
		block_shader.Activate();
		//			block_shader->bind("now", now);
		block_shader.Bind("view", cam.View);
		block_shader.Bind("proj", cam.Proj);
		// TODO You can probably derive this from the view matrix
		block_shader.Bind("camera_pos", cam.Position);
		block_manager.GetBlockTexture().Bind();
		block_manager.GetBlockStrTexture().Bind();
		shadowmap.Bind();
		world.Render(block_shader, cam);

		reticle_shader.Activate();
		Context<world::Reticle>::Get().Draw(reticle_shader, cam.View, cam.Proj,
		                                    cam.Position, cam.GetDirection(),
		                                    control::state.target
			                                    ? std::optional<glm::ivec3>(control::state.target->first)
			                                    : std::nullopt);

		tenbox_shader.Activate();
		// Remove translation from view matrix by truncation.
		tenbox_shader.Bind("view", glm::mat4(glm::mat3(cam.View)));
		tenbox_shader.Bind("proj", cam.Proj);
		Context<world::Skybox>::Get().Draw(tenbox_shader);

		glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		control::imgui_frame_end(m_window);

		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}
}

int main(int /*argc*/, char* /*argv*/[])
{
	spdlog::set_level(spdlog::level::debug);

	int code = glfwInit();
	assert(code);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	const auto m_window = glfwCreateWindow(INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT,
	                                       "renden", nullptr, nullptr);
	assert(m_window);

	glfwMakeContextCurrent(m_window);
	glfwSetMouseButtonCallback(m_window, control::mouse_button_callback);
	//glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
	glfwSetKeyCallback(m_window, control::key_callback);

	glfwSwapInterval(1);

	code = gladLoadGL();
	assert(code);
	spdlog::info("OpenGL {}", glGetString(GL_VERSION));

	glDebugMessageCallback(debug_callback, nullptr);
#ifndef NDEBUG
	glEnable(GL_DEBUG_OUTPUT);
#endif

	glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//	glLineWidth(10.f);

	init(m_window);
	control::imgui_init(m_window);
	loop(m_window);
	cleanup(m_window);
	control::imgui_cleanup();

	glfwTerminate();
	return EXIT_SUCCESS;
}
