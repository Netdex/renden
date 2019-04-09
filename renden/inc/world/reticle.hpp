#ifndef RENDEN_RETICLE_HPP
#define RENDEN_RETICLE_HPP

#include <memory>

#include "gl/mesh.hpp"
#include "loader/shader_manager.hpp"
#include "loader/block_manager.hpp""
#include "primitive/block_primitive.hpp"

class reticle {
	std::unique_ptr<gl::mesh> mesh;
public:
	reticle() : mesh(std::make_unique<gl::mesh>(gl::TRIANGLES, shaders::block::attribs)) {
		// this is a disgusting hack
		std::shared_ptr<block_primitive> block_primitive =
			world::entities::blocks::db.lock()->get_block_by_id(1);
		std::vector<float> vlist;
		block_primitive->append_vertex_list(vlist, glm::vec3(0, 0, 0),
			NEG_Z | NEG_Y | NEG_X | POS_Z | POS_Y | POS_X);
		mesh->buffer_vertex_data(vlist);
	}

	void draw(const gl::shader &shader, const glm::mat4 transform) {
		shader.bind("chunk", glm::mat4(1.0f));
		mesh->model = transform;
		GLint polyMode;
		glGetIntegerv(GL_POLYGON_MODE, &polyMode);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		mesh->draw(shader);
		//glPolygonMode(GL_FRONT_AND_BACK, polyMode);
	}
};

#endif