#ifndef RENDEN_CHUNK_HPP
#define RENDEN_CHUNK_HPP

#include <glm/vec3.hpp>

#include "gl/mesh.hpp"
#include "loader/shader_program.hpp"
#include "util/context.hpp"
#include "world/block.hpp"

namespace world
{
class Chunk
{

public:


	Chunk(glm::ivec3 location, std::array<Chunk*, 6> neighbors) : location_(location), neighbors_(neighbors),
	                                                              mesh_(std::make_unique<gl::Mesh<GLbyte>>(
		                                                              Context<shader::BlockShader>::Get().
		                                                              MeshAttributes, gl::POINTS))
	{
		const glm::vec3 translation = glm::vec3{chunk_to_world(location_, glm::ivec3{})};
		const glm::mat4 chunk = translate(glm::mat4(1.f), translation);
		mesh_->Model = chunk;
	}


	const Block& GetBlock(glm::ivec3 loc) const;
	void SetBlock(glm::ivec3 loc, const Block& block);

	Chunk* GetAdjacentChunk(Direction face) const { return neighbors_[dir_to_ord(face)]; }

	bool Update();
	void Draw(const gl::Shader& shader) const;

	void SetDirty(bool dirty) { dirty_ = dirty; }
	bool IsDirty() const { return dirty_; }

	std::pair<glm::vec3, glm::vec3> GetAABB() const;

	static glm::ivec3 world_to_chunk(const glm::ivec3& pos)
	{
		return glm::ivec3{floor(glm::vec3{pos} / float(kChunkWidth))};
	}

	static constexpr glm::ivec3 chunk_to_world(const glm::ivec3& chunk_pos, const glm::ivec3& loc)
	{
		return chunk_pos * kChunkWidth + loc;
	}

	static constexpr glm::ivec3 world_to_chunk_local(const glm::ivec3& pos)
	{
		// took me a few times to get this right
		return {
			(pos.x % kChunkWidth + kChunkWidth) % kChunkWidth,
			(pos.y % kChunkWidth + kChunkWidth) % kChunkWidth,
			(pos.z % kChunkWidth + kChunkWidth) % kChunkWidth
		};
	}

	static constexpr bool in_chunk_bounds(const glm::ivec3& loc)
	{
		return loc.x >= 0 && loc.x < kChunkWidth
			&& loc.y >= 0 && loc.y < kChunkWidth
			&& loc.z >= 0 && loc.z < kChunkWidth;
	}

	friend class World;

	static constexpr int kChunkWidth = 16;
private:
	bool face_occluded(glm::ivec3 position, Direction face) const;
	DirectionMask visible_faces(glm::ivec3 position) const;

	// y-x-z order
	Block data_[kChunkWidth][kChunkWidth][kChunkWidth];
	glm::ivec3 location_;
	std::array<Chunk*, 6> neighbors_;
	std::unique_ptr<gl::Mesh<GLbyte>> mesh_;
	bool dirty_ = false;
};
}


#endif //RENDEN_CHUNK_HPP
