
#include "world/reticle.hpp"

static const std::vector<float> reticle_points{
		-0.01, 0, 0,
		0.01, 0, 0,
		0, -0.01, 0,
		0, 0.01, 0
};

static const GLfloat CUBE_VERTICES[] = {
	// xy -z
	-0.5f, -0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f,
	// xy +z
	-0.5f, -0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f,
	-0.5f, -0.5f, 0.5f,
	// yz -x
	-0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f,
	// yz +x
	0.5f, 0.5f, 0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,
	// xz -y
	-0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,
	-0.5f, -0.5f, 0.5f,
	-0.5f, -0.5f, -0.5f,
	// xz +y
	-0.5f, 0.5f, -0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, -0.5f,
	0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, -0.5f,
	-0.5f, 0.5f, 0.5f,
};

reticle::reticle() :
	select_mesh(std::make_unique<gl::mesh>(CUBE_VERTICES, 3*6*6*sizeof(float), gl::TRIANGLES, &shaders::reticle::attribs[0], 1)),
	reticle_mesh(std::make_unique<gl::mesh>(reticle_points, gl::LINES, shaders::reticle::attribs)),
	dir_mesh(std::make_unique<gl::mesh>(gl::LINES, shaders::reticle::attribs))
{
}

void reticle::draw(const gl::shader& shader,
	const glm::mat4 &view, const glm::mat4 &proj,
	const glm::vec3 pos, const glm::vec3 dir,
	const std::optional<glm::ivec3> &target)
{
	shader.bind("model", glm::mat4(1.f));
	shader.bind("proj", proj);
	shader.bind("view", view);
	if (target) {
		select_mesh->model = glm::translate(glm::mat4(1.f), glm::vec3(*target));

		GLint polyMode;
		glGetIntegerv(GL_POLYGON_MODE, &polyMode);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		select_mesh->draw(shader);
		glPolygonMode(GL_FRONT_AND_BACK, polyMode);
	}

	/*float data[6] = { pos.x, pos.y, pos.z, pos.x + dir.x, pos.y + dir.y, pos.z + dir.z };
	dir_mesh->buffer_vertex_data(data, sizeof(float) * 6);
	dir_mesh->draw(shader);*/

	shader.bind("proj", glm::mat4(1.f));
	shader.bind("view", glm::mat4(1.f));
	reticle_mesh->draw(shader);
}
