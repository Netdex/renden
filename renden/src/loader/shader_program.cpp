#include "loader/shader_program.hpp"

#include "world/skybox.hpp"

namespace shader
{
BlockShader::BlockShader()
{
	shader_->Attach("block.vert").Attach("block.geom").Attach("block.frag").Link().Activate();
	const auto total = static_cast<unsigned int>((3 + 1) * sizeof(int) + (3 * 4) * sizeof(float));
	this->MeshAttributes = std::vector<gl::VArrayAttribute>{
		{shader_->GetAttribute("position"), 3, gl::INT, total, 0, false, 0},
		{
			shader_->GetAttribute("face"), 1, gl::INT, total,
			static_cast<unsigned int>(3 * sizeof(int)), false, 0
		},
		{
			shader_->GetAttribute("texcoords"), 3, gl::FLOAT, total,
			static_cast<unsigned int>(4 * sizeof(int)), false, 0
		},
		{
			shader_->GetAttribute("texcoords") + 1, 3, gl::FLOAT, total,
			static_cast<unsigned int>(4 * sizeof(int) + 3 * sizeof(float)), false, 0
		},
		{
			shader_->GetAttribute("texcoords") + 2, 3, gl::FLOAT, total,
			static_cast<unsigned int>(4 * sizeof(int) + 6 * sizeof(float)), false, 0
		},
		{
			shader_->GetAttribute("texcoords") + 3, 3, gl::FLOAT, total,
			static_cast<unsigned int>(4 * sizeof(int) + 9 * sizeof(float)), false, 0
		},
	};
	// the loaded texture should never change
	shader_->Bind("tex", 0);
	spdlog::debug("Loaded block shader");
}

SkyboxShader::SkyboxShader()
{
	shader_->Attach("skybox.vert").Attach("skybox.frag").Link().Activate();

	MeshAttributes = std::vector<gl::VArrayAttribute>{
		{
			shader_->GetAttribute("position"), 3, gl::FLOAT, 3 * sizeof(float),
			0, false, 0
		},
	};
	// the loaded texture should never change
	shader_->Bind("tex", 0);

	spdlog::debug("Loaded skybox shader");
}

ReticleShader::ReticleShader()
{
	shader_->Attach("reticle.vert").Attach("reticle.frag").Link().Activate();

	MeshAttributes = std::vector<gl::VArrayAttribute>{
		{
			shader_->GetAttribute("position"), 3, gl::FLOAT, 6 * sizeof(float),
			0, false, 0
		},
		{
			shader_->GetAttribute("color"), 3, gl::FLOAT, 6 * sizeof(float),
			3 * sizeof(float), false, 0
		},
	};
	spdlog::debug("Loaded reticle shader");
}
}
