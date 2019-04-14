#ifndef RENDEN_RETICLE_HPP
#define RENDEN_RETICLE_HPP

#include <memory>

#include "gl/mesh.hpp"
#include "loader/shader_manager.hpp"
#include "loader/block_manager.hpp""
#include "primitive/block_primitive.hpp"
#include <optional>

class reticle {
	std::unique_ptr<gl::mesh<>> select_mesh;
	std::unique_ptr<gl::mesh<>> reticle_mesh;
	std::unique_ptr<gl::mesh<>> dir_mesh;
	
public:
	reticle();

	void draw(const gl::shader& shader, 
		const glm::mat4 &view, const glm::mat4 &proj,
		const glm::vec3 pos, const glm::vec3 dir,
		const std::optional<glm::ivec3> &target);
};

#endif