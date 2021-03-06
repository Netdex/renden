#include "loader/shader_program.hpp"

#include "world/skybox.hpp"

namespace shader
{
BlockShader::BlockShader()
{
	shader_->Attach("block.vert").Attach("block.geom").Attach("block.frag").Link();
	auto scoped_lock = shader_->Use();
	const auto total = static_cast<unsigned int>((3 + 1) * sizeof(GLint) + sizeof(GLuint));
	this->MeshAttributes = std::vector<gl::VArrayAttribute>{
		{shader_->GetAttribute("position"), 3, gl::INT, total, 0, false, 0},
		{
			shader_->GetAttribute("face"), 1, gl::INT, total,
			static_cast<unsigned int>(3 * sizeof(GLint)), false, 0
		},
		{
			shader_->GetAttribute("tex_offset"), 1, gl::UINT, total,
			static_cast<unsigned int>(3 * sizeof(GLint) + sizeof(GLuint)), false, 0
		},
	};

	shader_->Bind("tex", kBlockTextureUnit);
	shader_->Bind("str_sampler", kStrTextureUnit);
	shader_->Bind("shadow_map", BlockDepthShader::kShadowmapTextureUnit);
	spdlog::debug("Loaded block shader");
}

BlockDepthShader::BlockDepthShader()
{
	shader_->Attach("block.vert").Attach("block.geom").Attach("block_depth.frag").Link();
	spdlog::debug("Loaded block depth shader");
}

SkyboxShader::SkyboxShader()
{
	shader_->Attach("skybox.vert").Attach("skybox.frag").Link();

	MeshAttributes = std::vector<gl::VArrayAttribute>{
		{
			shader_->GetAttribute("position"), 3, gl::FLOAT, 3 * sizeof(GLfloat),
			0, false, 0
		},
	};

	auto scoped_lock = shader_->Use();
	shader_->Bind("tex",kSkyTextureUnit);
	spdlog::debug("Loaded skybox shader");
}

ReticleShader::ReticleShader()
{
	shader_->Attach("reticle.vert").Attach("reticle.frag").Link();

	MeshAttributes = std::vector<gl::VArrayAttribute>{
		{
			shader_->GetAttribute("position"), 3, gl::FLOAT, 6 * sizeof(GLfloat),
			0, false, 0
		},
		{
			shader_->GetAttribute("color"), 3, gl::FLOAT, 6 * sizeof(GLfloat),
			3 * sizeof(GLfloat), false, 0
		},
	};
	spdlog::debug("Loaded reticle shader");
}
}
