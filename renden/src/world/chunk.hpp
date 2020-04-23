#ifndef RENDEN_CHUNK_HPP
#define RENDEN_CHUNK_HPP

#include <glm/vec3.hpp>

#include "gl/mesh.hpp"
#include "loader/shader_program.hpp"
#include "util/context.hpp"
#include "util/vec_iterator.hpp"
#include "world/block.hpp"

namespace world
{
class Chunk
{
public:
	class iterator : public util::vec_iterator<glm::ivec3, const Block&>
	{
	public:
		iterator(const Chunk& chunk, glm::ivec3 location, glm::ivec3 step) : vec_iterator(location, step), chunk_(chunk)
		{
		}

		const Block& operator*() override
		{
			return chunk_.GetBlockAt(position());
		}

	private:
		const Chunk& chunk_;
	};

	Chunk(glm::ivec3 location) : location_(location),
	                             mesh_(std::make_unique<gl::Mesh<GLbyte>>(
		                             Context<shader::BlockShader>::Get().MeshAttributes, gl::POINTS))
	{
	}

	const Block& GetBlockAt(glm::ivec3 loc) const;
	Block& GetBlockRefAt(glm::ivec3 loc, bool taint = true);

	bool UpdateMesh();
	void Draw(const gl::Shader& shader) const;

	std::pair<glm::vec3, glm::vec3> GetAABB() const;

	static glm::ivec3 block_to_chunk_pos(glm::ivec3 pos)
	{
		return glm::ivec3(
			int(floorf(float(pos.x) / kChunkWidth)),
			int(floorf(float(pos.y) / kChunkWidth)),
			int(floorf(float(pos.z) / kChunkWidth)));
	}

	static constexpr glm::ivec3 chunk_to_block_pos(glm::ivec3 chunk_pos, glm::ivec3 loc)
	{
		return chunk_pos * kChunkWidth + loc;
	}

	static constexpr glm::ivec3 block_to_loc_pos(glm::ivec3 pos)
	{
		// took me a few times to get this right
		return {
			(pos.x % kChunkWidth + kChunkWidth) % kChunkWidth,
			(pos.y % kChunkWidth + kChunkWidth) % kChunkWidth,
			(pos.z % kChunkWidth + kChunkWidth) % kChunkWidth
		};
	}

	static constexpr int kChunkWidth = 32;

private:
	bool face_occluded(glm::ivec3 position, world::BlockFace face) const;
	block_face_mask_t visible_faces(glm::ivec3 position) const;

	// y-z-x order
	Block data_[kChunkWidth][kChunkWidth][kChunkWidth];
	glm::ivec3 location_;
	std::unique_ptr<gl::Mesh<GLbyte>> mesh_;
	bool dirty_ = false;

	// Don't know whether this is a good idea yet.
	Chunk* adjacent_chunks_[6];
};
}


#endif //RENDEN_CHUNK_HPP
