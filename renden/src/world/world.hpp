#ifndef RENDEN_CHUNK_MANAGER_HPP
#define RENDEN_CHUNK_MANAGER_HPP

#include <unordered_map>
#include <optional>

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

	World() = default;


	Chunk* GetChunkAt(glm::ivec3 loc, bool create_if_not_exists = false);
	bool ChunkExists(glm::ivec3 loc);
	std::optional<Block> GetBlockAt(glm::ivec3 world_pos, bool create_if_not_exists = false);
	Block* GetBlockRefAt(glm::ivec3 world_pos, bool create_if_not_exists = false, bool taint = true);

	void Render(const gl::Shader& block_shader, const control::Camera& camera);
	void Update();

	static constexpr int kMaximumChunksPerUpdate = 10;
private:
	Chunk* CreateChunk(glm::ivec3 loc);

	std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>> chunks_;
};

}

#endif //RENDEN_CHUNK_MANAGER_HPP
