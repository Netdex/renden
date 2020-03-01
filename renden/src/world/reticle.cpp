#include "world/reticle.hpp"

static const std::vector<float> reticle_points{
	-0.01, 0, 0, 1, 1, 1,
	0.01, 0, 0, 0, 1, 1, 1,
	0, -0.01, 0, 1, 1, 1,
	0, 0.01, 0, 1, 1, 1,
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
	select_mesh_(std::make_unique<gl::Mesh<>>(CUBE_VERTICES, sizeof(CUBE_VERTICES), gl::TRIANGLES,
	                                         &shaders::reticle::attribs[0], 2)),
	reticle_mesh_(std::make_unique<gl::Mesh<>>(reticle_points, gl::LINES, shaders::reticle::attribs)),
	dir_mesh_(std::make_unique<gl::Mesh<>>(gl::LINES, shaders::reticle::attribs))
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
	dir_mesh_->BufferVertexData(&axis[0][0], sizeof(axis));
	dir_mesh_->Draw(shader);

	shader.Bind("proj", glm::mat4(1.f));
	shader.Bind("view", glm::mat4(1.f));
	reticle_mesh_->Draw(shader);
}
