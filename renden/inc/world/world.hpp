#ifndef RENDEN_CHUNK_MANAGER_HPP
#define RENDEN_CHUNK_MANAGER_HPP

#include <map>
#include <optional>

#include <glm/vec2.hpp>
#include <spdlog/spdlog.h>

#include "world/chunk.hpp"

namespace world
{
class World
{
	std::map<std::pair<int, int>, std::shared_ptr<Chunk>> chunks_;

public:
	World() = default;

	std::shared_ptr<Chunk> GetChunkAt(int x, int z)
	{
		// if chunk does not exist, it will be default constructed
		auto& ptr = chunks_[std::make_pair(x, z)];
		if (!ptr)
		{
			spdlog::debug("generated chunk ({},{}) due to first time access", x, z);
			ptr = std::make_shared<Chunk>(glm::ivec2{x, z});
		}
		return ptr;
	}

	bool ChunkExists(int x, int z)
	{
		return chunks_.find(std::make_pair(x, z)) != chunks_.end();
	}

	void Render(const gl::Shader& block_shader)
	{
		for (const auto& chunk : chunks_)
		{
			chunk.second->Draw(block_shader);
		}
	}

	std::optional<Block> GetBlockAt(glm::ivec3 world_pos, bool create_if_not_exists = false)
	{
		const glm::ivec2 chunk_pos = Chunk::block_to_chunk_pos(world_pos);
		if (!create_if_not_exists && !ChunkExists(chunk_pos.x, chunk_pos.y))
			return std::nullopt;

		glm::ivec3 loc_pos = Chunk::block_to_loc_pos(world_pos);
		return GetChunkAt(chunk_pos.x, chunk_pos.y)->GetBlockAt(loc_pos.x, loc_pos.y, loc_pos.z);
	}

	std::optional<std::reference_wrapper<Block>> GetBlockRefAt(glm::ivec3 world_pos,
	                                                           bool create_if_not_exists = false, bool taint = true)
	{
		const glm::ivec2 chunk_pos = Chunk::block_to_chunk_pos(world_pos);
		if (!create_if_not_exists && !ChunkExists(chunk_pos.x, chunk_pos.y))
			return std::nullopt;
		glm::ivec3 loc_pos = Chunk::block_to_loc_pos(world_pos);

		spdlog::debug("block ref: ({},{},{}) -> ({},{}) ({},{},{})",
		              world_pos.x, world_pos.y, world_pos.z,
		              chunk_pos.x, chunk_pos.y, loc_pos.x, loc_pos.y, loc_pos.z);

		return GetChunkAt(chunk_pos.x, chunk_pos.y)->GetBlockRefAt(loc_pos.x, loc_pos.y, loc_pos.z, taint);
	}

	void Update()
	{
		// TODO Maybe lazily stagger chunks that need updating across multiple frames?
		for (auto cnk : chunks_)
			cnk.second->UpdateMesh();
	}
};
}

#endif //RENDEN_CHUNK_MANAGER_HPP
