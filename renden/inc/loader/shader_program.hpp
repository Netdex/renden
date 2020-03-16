#pragma once
#include <memory>

#include "gl/shader.hpp"
#include "gl/varray.hpp"
#include "world/skybox.hpp"

namespace shader {
class ShaderProgram
{
protected:
	std::shared_ptr<gl::Shader> shader_;
public:
	ShaderProgram() : shader_(std::make_shared<gl::Shader>())
	{
	}

	virtual ~ShaderProgram() = default;

	ShaderProgram(const ShaderProgram& o) = delete;
	ShaderProgram& operator=(const ShaderProgram& o) = delete;

	std::shared_ptr<gl::Shader> GetShader() const
	{
		return shader_;
	}
};


class BlockShader : public ShaderProgram
{
public:
	BlockShader();

	std::vector<gl::VArrayAttribute> MeshAttributes;
};

class SkyboxShader : public ShaderProgram
{
public:
	SkyboxShader();

	std::vector<gl::VArrayAttribute> MeshAttributes;
};

class ReticleShader : public ShaderProgram
{
public:
	ReticleShader();

	std::vector<gl::VArrayAttribute> MeshAttributes;
};

}
