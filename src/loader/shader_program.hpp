#pragma once
#include <memory>

#include "gl/shader.hpp"
#include "gl/varray.hpp"
#include "world/skybox.hpp"

namespace shader
{
class ShaderProgram
{
protected:
	std::unique_ptr<gl::Shader> shader_;
public:
	ShaderProgram() : shader_(std::make_unique<gl::Shader>())
	{
	}

	virtual ~ShaderProgram() = default;

	ShaderProgram(const ShaderProgram& o) = delete;
	ShaderProgram& operator=(const ShaderProgram& o) = delete;

	gl::Shader& GetShader() const
	{
		return *shader_;
	}
};

class BlockShader : public ShaderProgram
{
public:
	BlockShader();

	std::vector<gl::VArrayAttribute> MeshAttributes;

	static constexpr GLint kBlockTextureUnit = 0;
	static constexpr GLint kStrTextureUnit = 2;
};

/// \brief A BlockShader but with an empty fragment shader.
class BlockDepthShader : public ShaderProgram
{
public:
	BlockDepthShader();

	// Shares mesh attributes with BlockShader.

	static constexpr GLint kShadowmapTextureUnit = 3;
};

class SkyboxShader : public ShaderProgram
{
public:
	SkyboxShader();

	std::vector<gl::VArrayAttribute> MeshAttributes;

	static constexpr GLint kSkyTextureUnit = 1;
};

class ReticleShader : public ShaderProgram
{
public:
	ReticleShader();

	std::vector<gl::VArrayAttribute> MeshAttributes;
};
}
