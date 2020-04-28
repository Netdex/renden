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
	class const_iterator
	{
		class axis_functor
		{
		public:
			axis_functor(const_iterator& it, int component);

			axis_functor& operator++();
			axis_functor& operator--();

		private:
			const_iterator& it_;
			int component_;
		};

	public:
		const_iterator(const Chunk* chunk, glm::ivec3 location) :
			x(*this, 0),
			y(*this, 1),
			z(*this, 2),
			local_pos_(location),
			chunk_(chunk)
		{
			assert(local_pos_.x >= 0 && local_pos_.x < kChunkWidth
				&& local_pos_.z >= 0 && local_pos_.z < kChunkWidth
				&& local_pos_.y >= 0 && local_pos_.y < kChunkWidth);
		}

		void step(BlockFace face);

		const Block& operator*() const
		{
			assert(this->valid());
			return chunk_->GetBlockAt(local_pos_);
		}

		bool valid() const { return chunk_ != nullptr; }
		glm::ivec3 world_pos() const { return chunk_to_block_pos(chunk_->location_, local_pos_); }

		axis_functor x, y, z;

	private:
		glm::ivec3 local_pos_;
		const Chunk* chunk_;
	};

	Chunk(glm::ivec3 location, std::array<Chunk*, 6> neighbors) : location_(location), neighbors_(neighbors),
	                                                              mesh_(std::make_unique<gl::Mesh<GLbyte>>(
		                                                              Context<shader::BlockShader>::Get().
		                                                              MeshAttributes, gl::POINTS))
	{
		const glm::vec3 translation = glm::vec3{chunk_to_block_pos(location_, glm::ivec3{})};
		const glm::mat4 chunk = translate(glm::mat4(1.f), translation);
		mesh_->Model = chunk;
	}

	const_iterator begin(glm::ivec3 loc) const { return const_iterator{this, loc}; }

	const Block& GetBlockAt(glm::ivec3 loc) const;
	Block& GetBlockRefAt(glm::ivec3 loc, bool taint = true);
	Chunk* GetAdjacentChunk(BlockFace face) const { return neighbors_[kFaceToIndex[face]]; }

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

	friend class World;

	static constexpr int kChunkWidth = 16;
private:
	bool face_occluded(glm::ivec3 position, BlockFace face) const;
	block_face_mask_t visible_faces(glm::ivec3 position) const;

	// y-z-x order
	Block data_[kChunkWidth][kChunkWidth][kChunkWidth];
	glm::ivec3 location_;
	std::array<Chunk*, 6> neighbors_;
	std::unique_ptr<gl::Mesh<GLbyte>> mesh_;
	bool dirty_ = false;
};

inline Chunk::const_iterator::axis_functor::
axis_functor(const_iterator& it, int component) : it_(it), component_(component)
{
}

inline void Chunk::const_iterator::step(BlockFace face)
{
	switch (face)
	{
	case NEG_Z: --z;
		break;
	case POS_Z: ++z;
		break;
	case NEG_X: --x;
		break;
	case POS_X: ++x;
		break;
	case NEG_Y: --y;
		break;
	case POS_Y: ++y;
		break;
	default: ;
	}
}

inline Chunk::const_iterator::axis_functor& Chunk::const_iterator::axis_functor::operator++()
{
	constexpr BlockFace adj_face[] = {POS_X, POS_Y, POS_Z};
	it_.local_pos_[component_]++;
	if (it_.local_pos_[component_] >= kChunkWidth)
	{
		it_.local_pos_[component_] -= kChunkWidth;
		it_.chunk_ = it_.chunk_->GetAdjacentChunk(adj_face[component_]);
	}
	return *this;
}

inline Chunk::const_iterator::axis_functor& Chunk::const_iterator::axis_functor::operator--()
{
	constexpr BlockFace adj_face[] = {NEG_X, NEG_Y, NEG_Z};
	it_.local_pos_[component_]--;
	if (it_.local_pos_[component_] < 0)
	{
		it_.local_pos_[component_] += kChunkWidth;
		it_.chunk_ = it_.chunk_->GetAdjacentChunk(adj_face[component_]);
	}
	return *this;
}
}


#endif //RENDEN_CHUNK_HPP
