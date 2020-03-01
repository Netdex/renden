//
// Created by netdex on 2/17/19.
//

#include <memory>

#include <loader/shader_manager.hpp>
#include "spdlog/spdlog.h"

namespace shaders
{
namespace block
{
	std::weak_ptr<gl::Shader> shader;

	std::vector<gl::VArrayAttribute> attribs;

	std::shared_ptr<gl::Shader> Load()
	{
		auto shr_shader = std::make_shared<gl::Shader>();
		shader = shr_shader;
		shr_shader->Attach("block.vert").Attach("block.geom").Attach("block.frag").Link();

		unsigned int total = static_cast<unsigned int>((3 + 1) * sizeof(int) + (3 * 4) * sizeof(float));
		attribs = std::vector<gl::VArrayAttribute>{
			{0, 3, gl::INT, total, 0, false, 0},
			{1, 1, gl::INT, total, static_cast<unsigned int>(3 * sizeof(int)), false, 0},
			{2, 3, gl::FLOAT, total, static_cast<unsigned int>(4 * sizeof(int)), false, 0},
			{2 + 1, 3, gl::FLOAT, total, static_cast<unsigned int>(4 * sizeof(int) + 3 * sizeof(float)), false, 0},
			{2 + 2, 3, gl::FLOAT, total, static_cast<unsigned int>(4 * sizeof(int) + 6 * sizeof(float)), false, 0},
			{2 + 3, 3, gl::FLOAT, total, static_cast<unsigned int>(4 * sizeof(int) + 9 * sizeof(float)), false, 0},
		};
		// the loaded texture should never change
		shr_shader->Bind("tex", 0);
		spdlog::debug("loaded block shader");
		return shr_shader;
	}
}

namespace tenbox
{
	std::weak_ptr<gl::Shader> shader;
	std::vector<gl::VArrayAttribute> attribs;
	std::shared_ptr<Skybox> tenbox;

	std::shared_ptr<gl::Shader> Load()
	{
		auto shr_shader = std::make_shared<gl::Shader>();
		shader = shr_shader;
		shr_shader->Attach("skybox.vert").Attach("skybox.frag").Link();

		attribs = std::vector<gl::VArrayAttribute>{
			{shr_shader->GetAttribute("position"), 3, gl::FLOAT, 3 * sizeof(float), 0, false, 0},
		};
		// the loaded texture should never change
		shr_shader->Bind("tex", 0);

		tenbox = std::make_shared<Skybox>(std::vector<std::string>{
			PROJECT_SOURCE_DIR "/renden/res/skybox/alps_rt.tga",
			PROJECT_SOURCE_DIR "/renden/res/skybox/alps_lf.tga",
			PROJECT_SOURCE_DIR "/renden/res/skybox/alps_up.tga",
			PROJECT_SOURCE_DIR "/renden/res/skybox/alps_dn.tga",
			PROJECT_SOURCE_DIR "/renden/res/skybox/alps_bk.tga",
			PROJECT_SOURCE_DIR "/renden/res/skybox/alps_ft.tga",
		});

		spdlog::debug("loaded skybox shader");

		return shr_shader;
	}
}

namespace reticle
{
	std::weak_ptr<gl::Shader> shader;
	std::vector<gl::VArrayAttribute> attribs;

	std::shared_ptr<gl::Shader> Load()
	{
		auto shr_shader = std::make_shared<gl::Shader>();
		shader = shr_shader;
		shr_shader->Attach("reticle.vert").Attach("reticle.frag").Link();

		attribs = std::vector<gl::VArrayAttribute>{
			{shr_shader->GetAttribute("position"), 3, gl::FLOAT, 6 * sizeof(float), 0, false, 0},
			{shr_shader->GetAttribute("color"), 3, gl::FLOAT, 6 * sizeof(float), 3 * sizeof(float), false, 0},
		};
		// the loaded texture should never change
		spdlog::debug("loaded reticle shader");
		return shr_shader;
	}
}

std::vector<std::shared_ptr<gl::Shader>> Load()
{
	auto block_shader = block::Load();
	auto skybox_shader = tenbox::Load();
	auto reticle_shader = reticle::Load();
	return {block_shader, skybox_shader, reticle_shader};
}
}
