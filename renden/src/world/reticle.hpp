#ifndef RENDEN_RETICLE_HPP
#define RENDEN_RETICLE_HPP

#include <memory>
#include <optional>

#include "gl/mesh.hpp"
#include "loader/block_manager.hpp"
#include "primitive/block_primitive.hpp"

namespace world
{
class Reticle
{
	std::unique_ptr<gl::Mesh<>> select_mesh_;
	std::unique_ptr<gl::Mesh<>> reticle_mesh_;
	std::unique_ptr<gl::Mesh<>> dir_mesh_;

public:
	Reticle();

	void Draw(const gl::Shader& shader,
	          const glm::mat4& view, const glm::mat4& proj,
	          const glm::vec3& pos, const glm::vec3& dir,
	          const std::optional<glm::ivec3>& target) const;
};
}

#endif
