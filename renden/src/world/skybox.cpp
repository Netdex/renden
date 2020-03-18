#include "world/skybox.hpp"

#include <glad/glad.h>

#include "loader/shader_program.hpp"
#include "util/context.hpp"

namespace world
{
const GLfloat SKYBOX_VERTICES[] = {
	// xy -z (FRONT)
	-1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	// yz -x (LEFT)
	-1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	// yz +x (RIGHT)
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	// xy +z (BACK)
	-1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	// xz +y (TOP)
	-1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	// xz -y (BOTTOM)
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,
};

Skybox::Skybox(const std::vector<std::string>& paths)
	: texture_(std::make_shared<gl::Cubemap>(paths, gl::LINEAR, gl::CLAMP_EDGE, shader::SkyboxShader::kSkyTextureUnit)),
	  mesh_(std::make_shared<gl::Mesh<>>(SKYBOX_VERTICES,
	                                     Context<shader::SkyboxShader>::Get().MeshAttributes, gl::TRIANGLES))
{
}

void Skybox::Draw(const gl::Shader& shader) const
{
	GLint depth_func;
	glGetIntegerv(GL_DEPTH_FUNC, &depth_func);
	glDepthFunc(GL_LEQUAL);
	mesh_->Draw(shader);
	glDepthFunc(depth_func);
}
}
