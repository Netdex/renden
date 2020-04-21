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
	struct ivec3_map_predicate
	{
		bool operator()(const glm::ivec3& a, const glm::ivec3& b) const;
	};

	std::map<glm::ivec3, std::unique_ptr<Chunk>, ivec3_map_predicate> chunks_;

public:
	World() = default;

	Chunk* GetChunkAt(glm::ivec3 loc, bool create_if_not_exists = false);
	bool ChunkExists(glm::ivec3 loc);
	std::optional<Block> GetBlockAt(glm::ivec3 world_pos, bool create_if_not_exists = false);
	Block* GetBlockRefAt(glm::ivec3 world_pos, bool create_if_not_exists = false, bool taint = true);

	void Render(const gl::Shader& block_shader, const control::Camera &camera);
	void Update();

	static constexpr int kMaximumChunksPerUpdate = 10;
};
}

#endif //RENDEN_CHUNK_MANAGER_HPP
