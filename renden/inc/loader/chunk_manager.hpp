#ifndef RENDEN_CHUNK_MANAGER_HPP
#define RENDEN_CHUNK_MANAGER_HPP

#include <map>
#include <optional>

#include <glm/vec2.hpp>
#include <spdlog/spdlog.h>

#include "world/chunk.hpp"

template <int W, int H>
class ChunkManager
{
	std::map<std::pair<int, int>, std::shared_ptr<Chunk<W, H>>> chunks_;

public:
	ChunkManager() = default;

	std::shared_ptr<Chunk<W, H>> GetChunkAt(int x, int z)
	{
		// if chunk does not exist, it will be default constructed
		auto& ptr = chunks_[std::make_pair(x, z)];
		if (!ptr)
		{
			spdlog::debug("generated chunk ({},{}) due to first time access", x, z);
			ptr = std::make_shared<Chunk<W, H>>();
		}
		return ptr;
	}

	bool ChunkExists(int x, int z)
	{
		return chunks_.find(std::make_pair(x, z)) != chunks_.end();
	}

	void Render(const gl::Shader& block)
	{
		for (auto chunk : chunks_)
		{
			chunk.second->Draw(block, glm::translate(glm::mat4(1.f),
			                                         glm::vec3(chunk.first.first * int(W), 0,
			                                                   chunk.first.second * int(W))));
		}
	}

	static constexpr glm::ivec2 block_to_chunk_pos(glm::ivec3 pos)
	{
		return glm::ivec2(int(floorf(float(pos.x) / W)), int(floorf(float(pos.z) / W)));
	}

	static constexpr glm::ivec3 chunk_to_block_pos(glm::ivec2 chunk_pos, glm::ivec3 loc)
	{
		return glm::ivec3(chunk_pos.x * W + loc.x, loc.y, chunk_pos.y * W + loc.z);
	}

	static constexpr glm::ivec3 block_to_loc_pos(glm::ivec3 pos)
	{
		// took me a few times to get this right
		return {(pos.x % W + W) % W, pos.y, (pos.z % W + W) % W};
	}

	std::optional<Block> GetBlockAt(glm::ivec3 world_pos, bool create_if_not_exists = false)
	{
		const glm::ivec2 chunk_pos = block_to_chunk_pos(world_pos);
		if (!create_if_not_exists && !ChunkExists(chunk_pos.x, chunk_pos.y))
			return std::nullopt;

		glm::ivec3 loc_pos = block_to_loc_pos(world_pos);
		return GetChunkAt(chunk_pos.x, chunk_pos.y)->GetBlockAt(loc_pos.x, loc_pos.y, loc_pos.z);
	}

	std::optional<std::reference_wrapper<Block>> GetBlockRefAt(glm::ivec3 world_pos,
	                                                              bool create_if_not_exists = false, bool taint = true)
	{
		const glm::ivec2 chunk_pos = block_to_chunk_pos(world_pos);
		if (!create_if_not_exists && !ChunkExists(chunk_pos.x, chunk_pos.y))
			return std::nullopt;
		glm::ivec3 loc_pos = block_to_loc_pos(world_pos);

		spdlog::debug("block ref: ({},{},{}) -> ({},{}) ({},{},{})",
		              world_pos.x, world_pos.y, world_pos.z,
		              chunk_pos.x, chunk_pos.y, loc_pos.x, loc_pos.y, loc_pos.z);

		return GetChunkAt(chunk_pos.x, chunk_pos.y)->GetBlockRefAt(loc_pos.x, loc_pos.y, loc_pos.z, taint);
	}

	void Update()
	{
		for (auto cnk : chunks_)
			cnk.second->UpdateMesh();
	}
};

namespace world::chunk
{
const int CHUNK_W = 16;
const int CHUNK_H = 128;

typedef ChunkManager<CHUNK_W, CHUNK_H> chunk_mgr_t;

extern std::weak_ptr<chunk_mgr_t> db;

std::shared_ptr<chunk_mgr_t> load();
}

#endif //RENDEN_CHUNK_MANAGER_HPP
