#ifndef RENDEN_CHUNK_MANAGER_HPP
#define RENDEN_CHUNK_MANAGER_HPP

#include <unordered_map>

#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#undef  GLM_ENABLE_EXPERIMENTAL

#include "control/camera.hpp"
#include "world/chunk.hpp"

namespace world
{
class World
{
public:
	class const_iterator
	{
		class axis
		{
		public:
			axis(World& world, const_iterator& it, int component) : world_(world), it_(it), component_(component)
			{
			}

			axis& operator++()
			{
				constexpr Direction adj_face[] = {POS_X, POS_Y, POS_Z};
				++it_.world_pos_[component_];
				if (Chunk::world_to_chunk_local(it_.world_pos_)[component_] == 0)
				{
					if (it_.chunk_)
						it_.chunk_ = it_.chunk_->GetAdjacentChunk(adj_face[component_]);
					else
						it_.chunk_ = world_.GetChunk(Chunk::world_to_chunk(it_.world_pos_));
				}
				return *this;
			}

			axis& operator--()
			{
				constexpr Direction adj_face[] = {NEG_X, NEG_Y, NEG_Z};
				--it_.world_pos_[component_];
				if (Chunk::world_to_chunk_local(it_.world_pos_)[component_] == Chunk::kChunkWidth - 1)
				{
					if (it_.chunk_)
						it_.chunk_ = it_.chunk_->GetAdjacentChunk(adj_face[component_]);
					else
						it_.chunk_ = world_.GetChunk(Chunk::world_to_chunk(it_.world_pos_));
				}
				return *this;
			}

		private:
			World& world_;
			const_iterator& it_;
			int component_;
		};

	public:

		const_iterator(World& world, glm::ivec3 location) :
			x(world, *this, 0),
			y(world, *this, 1),
			z(world, *this, 2),
			world_(world),
			world_pos_(location),
			chunk_(world.GetChunk(Chunk::world_to_chunk(location)))
		{
		}

		void step(Direction face)
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

		const Block* operator*() const
		{
			if (chunk_)
				return &chunk_->GetBlock(local());
			return nullptr;
		}

		Chunk* chunk(bool create = false) const
		{
			if (chunk_)
				return chunk_;
			if (create)
				return world_.GetChunk(Chunk::world_to_chunk(world_pos_), true);
			return nullptr;
		}

		bool exists() const { return chunk_ != nullptr; }
		glm::ivec3 position() const { return world_pos_; }
		glm::ivec3 local() const { return Chunk::world_to_chunk_local(world_pos_); }
		void set(const Block& block, bool create) const { chunk(create)->SetBlock(local(), block); }

		axis x, y, z;

	private:
		World& world_;
		glm::ivec3 world_pos_;
		Chunk* chunk_;
	};

	World() = default;

	const_iterator cbegin(glm::ivec3 pos) { return const_iterator{*this, pos}; }

	Chunk* GetChunk(glm::ivec3 loc, bool create_if_not_exists = false);
	bool ChunkExists(glm::ivec3 loc);
	const Block* GetBlock(glm::ivec3 world_pos);
	void SetBlock(glm::ivec3 world_pos, const Block& block);

	void Render(const gl::Shader& block_shader, const control::Camera& camera, bool frustum_cull = true);
	void Update();

	static constexpr int kMaximumChunksPerUpdate = 10;
private:
	Chunk* CreateChunk(glm::ivec3 loc);

	std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>> chunks_;
};
}

#endif //RENDEN_CHUNK_MANAGER_HPP
