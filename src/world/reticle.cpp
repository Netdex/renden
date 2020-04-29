#include "world/reticle.hpp"

#include <glad/glad.h>

#include "loader/shader_program.hpp"
#include "util/context.hpp"

namespace world
{
static const GLfloat RETICLE_POINTS[] = {
	-0.01f, 0.f, 0.f, 1.f, 1.f, 1.f,
	0.01f, 0.f, 0.f, 1.f, 1.f, 1.f,
	0.f, -0.01f, 0.f, 1.f, 1.f, 1.f,
	0.f, 0.01f, 0.f, 1.f, 1.f, 1.f,
};

static const GLfloat CUBE_VERTICES[] = {
	// xy -z
	-0.5f, -0.5f, -0.5f, 1, 1, 1,
	0.5f, -0.5f, -0.5f, 1, 1, 1,
	0.5f, 0.5f, -0.5f, 1, 1, 1,
	0.5f, 0.5f, -0.5f, 1, 1, 1,
	-0.5f, 0.5f, -0.5f, 1, 1, 1,
	-0.5f, -0.5f, -0.5f, 1, 1, 1,
	// xy +z
	-0.5f, -0.5f, 0.5f, 1, 1, 1,
	0.5f, 0.5f, 0.5f, 1, 1, 1,
	0.5f, -0.5f, 0.5f, 1, 1, 1,
	0.5f, 0.5f, 0.5f, 1, 1, 1,
	-0.5f, -0.5f, 0.5f, 1, 1, 1,
	-0.5f, 0.5f, 0.5f, 1, 1, 1,
	// yz -x
	-0.5f, 0.5f, 0.5f, 1, 1, 1,
	-0.5f, -0.5f, -0.5f, 1, 1, 1,
	-0.5f, 0.5f, -0.5f, 1, 1, 1,
	-0.5f, -0.5f, -0.5f, 1, 1, 1,
	-0.5f, 0.5f, 0.5f, 1, 1, 1,
	-0.5f, -0.5f, 0.5f, 1, 1, 1,
	// yz +x
	0.5f, 0.5f, 0.5f, 1, 1, 1,
	0.5f, 0.5f, -0.5f, 1, 1, 1,
	0.5f, -0.5f, -0.5f, 1, 1, 1,
	0.5f, -0.5f, -0.5f, 1, 1, 1,
	0.5f, -0.5f, 0.5f, 1, 1, 1,
	0.5f, 0.5f, 0.5f, 1, 1, 1,
	// xz -y
	-0.5f, -0.5f, -0.5f, 1, 1, 1,
	0.5f, -0.5f, 0.5f, 1, 1, 1,
	0.5f, -0.5f, -0.5f, 1, 1, 1,
	0.5f, -0.5f, 0.5f, 1, 1, 1,
	-0.5f, -0.5f, -0.5f, 1, 1, 1,
	-0.5f, -0.5f, 0.5f, 1, 1, 1,
	// xz +y
	-0.5f, 0.5f, -0.5f, 1, 1, 1,
	0.5f, 0.5f, -0.5f, 1, 1, 1,
	0.5f, 0.5f, 0.5f, 1, 1, 1,
	0.5f, 0.5f, 0.5f, 1, 1, 1,
	-0.5f, 0.5f, 0.5f, 1, 1, 1,
	-0.5f, 0.5f, -0.5f, 1, 1, 1,
};

Reticle::Reticle() :
	select_mesh_(std::make_unique<gl::Mesh<>>(CUBE_VERTICES,
	                                          Context<shader::ReticleShader>::Get().MeshAttributes, gl::TRIANGLES)),
	reticle_mesh_(std::make_unique<gl::Mesh<>>(RETICLE_POINTS, Context<shader::ReticleShader>::Get().MeshAttributes,
	                                           gl::LINES)),
	dir_mesh_(std::make_unique<gl::Mesh<>>(Context<shader::ReticleShader>::Get().MeshAttributes, gl::LINES))
{
}

void Reticle::Draw(const gl::Shader& shader,
                   const glm::mat4& view, const glm::mat4& proj,
                   const glm::vec3& pos, const glm::vec3& dir,
                   const std::optional<glm::ivec3>& target) const
{
	shader.Bind("model", glm::mat4(1.f));
	shader.Bind("proj", proj);
	shader.Bind("view", view);
	if (target)
	{
		// Scale the block ghost to be slightly larger than a block, so that there isn't z-fighting. Sorry.
		select_mesh_->Model = glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(*target)),
		                                 glm::vec3(1.f) + 0.01f);

		//GLint polyMode;
		//glGetIntegerv(GL_POLYGON_MODE, &polyMode);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		select_mesh_->Draw(shader);
		//glPolygonMode(GL_FRONT_AND_BACK, polyMode);
	}

	glm::vec3 o = pos + glm::normalize(dir) * 0.5f;
	glm::vec3 axis[] = {
		o, glm::vec3(1, 0, 0),
		o + glm::vec3(1, 0, 0), glm::vec3(1, 0, 0),
		o, glm::vec3(0, 1, 0),
		o + glm::vec3(0, 1, 0), glm::vec3(0, 1, 0),
		o, glm::vec3(0, 0, 1),
		o + glm::vec3(0, 0, 1), glm::vec3(0, 0, 1),
	};

	dir_mesh_->Model = glm::mat4(1.f);
	// TODO This is kind of nasty
	dir_mesh_->BufferVertexData(nonstd::span(&axis[0][0], sizeof(axis) / (sizeof(float))));
	dir_mesh_->Draw(shader);

	shader.Bind("proj", glm::mat4(1.f));
	shader.Bind("view", glm::mat4(1.f));
	reticle_mesh_->Draw(shader);
}
}
