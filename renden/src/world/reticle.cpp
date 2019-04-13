
#include "world/reticle.hpp"

static const std::vector<float> reticle_points{
		-0.01,	0, 		0,	1,	1,  1,
		0.01,	0, 0,	0,	1,  1,	1,
		0,		-0.01,	0,	1,	1,  1,
		0,		0.01,	0,	1,	1,  1,
};

static const GLfloat CUBE_VERTICES[] = {
	// xy -z
	-0.5f, -0.5f, -0.5f,	  1,1,1,
	0.5f, 0.5f, -0.5f,		  1,1,1,
	0.5f, -0.5f, -0.5f,		  1,1,1,
	0.5f, 0.5f, -0.5f,		  1,1,1,
	-0.5f, -0.5f, -0.5f,	  1,1,1,
	-0.5f, 0.5f, -0.5f,		  1,1,1,
	// xy +z				  
	-0.5f, -0.5f, 0.5f,		  1,1,1,
	0.5f, -0.5f, 0.5f,		  1,1,1,
	0.5f, 0.5f, 0.5f,		  1,1,1,
	0.5f, 0.5f, 0.5f,		  1,1,1,
	-0.5f, 0.5f, 0.5f,		  1,1,1,
	-0.5f, -0.5f, 0.5f,		  1,1,1,
	// yz -x				  
	-0.5f, 0.5f, 0.5f,		  1,1,1,
	-0.5f, 0.5f, -0.5f,		  1,1,1,
	-0.5f, -0.5f, -0.5f,	  1,1,1,
	-0.5f, -0.5f, -0.5f,	  1,1,1,
	-0.5f, -0.5f, 0.5f,		  1,1,1,
	-0.5f, 0.5f, 0.5f,		  1,1,1,
	// yz +x				  
	0.5f, 0.5f, 0.5f,		  1,1,1,
	0.5f, -0.5f, -0.5f,		  1,1,1,
	0.5f, 0.5f, -0.5f,		  1,1,1,
	0.5f, -0.5f, -0.5f,		  1,1,1,
	0.5f, 0.5f, 0.5f,		  1,1,1,
	0.5f, -0.5f, 0.5f,		  1,1,1,
	// xz -y				 
	-0.5f, -0.5f, -0.5f,	  1,1,1,
	0.5f, -0.5f, -0.5f,		  1,1,1,
	0.5f, -0.5f, 0.5f,		  1,1,1,
	0.5f, -0.5f, 0.5f,		  1,1,1,
	-0.5f, -0.5f, 0.5f,		  1,1,1,
	-0.5f, -0.5f, -0.5f,	  1,1,1,
	// xz +y				 
	-0.5f, 0.5f, -0.5f,		  1,1,1,
	0.5f, 0.5f, 0.5f,		  1,1,1,
	0.5f, 0.5f, -0.5f,		  1,1,1,
	0.5f, 0.5f, 0.5f,		  1,1,1,
	-0.5f, 0.5f, -0.5f,		  1,1,1,
	-0.5f, 0.5f, 0.5f,		  1,1,1,
};							  

reticle::reticle() :
	select_mesh(std::make_unique<gl::mesh>(CUBE_VERTICES, sizeof(CUBE_VERTICES), gl::TRIANGLES, &shaders::reticle::attribs[0], 2)),
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

	glm::vec3 o = pos + glm::normalize(dir)*0.5f;
	glm::vec3 axis[] = {
		o,						glm::vec3(1,0,0), 
		o + glm::vec3(1,0,0),	glm::vec3(1,0,0),
		o,						glm::vec3(0,1,0),
		o + glm::vec3(0,1,0),	glm::vec3(0,1,0),
		o,						glm::vec3(0,0,1),
		o + glm::vec3(0,0,1),	glm::vec3(0,0,1),
	};

	dir_mesh->model = glm::mat4(1.f);
	dir_mesh->buffer_vertex_data(&axis[0][0], sizeof(axis));
	dir_mesh->draw(shader);

	shader.bind("proj", glm::mat4(1.f));
	shader.bind("view", glm::mat4(1.f));
	reticle_mesh->draw(shader);
}
