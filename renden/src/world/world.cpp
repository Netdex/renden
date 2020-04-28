#include "world/world.hpp"

#include "util/math.hpp"

namespace world
{

Chunk* World::GetChunkAt(glm::ivec3 loc, bool create_if_not_exists)
{
	const auto it = chunks_.find(loc);

	if (it == chunks_.end())
	{
		if (create_if_not_exists)
			return CreateChunk(loc);
		return nullptr;
	}
	auto& [key, value] = *it;
	return value.get();
}

Chunk* World::CreateChunk(glm::ivec3 loc)
{
	auto& ptr = chunks_[loc];
	std::array<Chunk*, 6> neighbors{};
	for (int i = 0; i < 6; ++i)
	{
		glm::ivec3 offset = kFaceToOffset[i];
		Chunk* adj = GetChunkAt(loc + offset);
		neighbors[i] = adj;
		if (adj)
			adj->neighbors_[kFaceToIndex[kFaceToBlockInv[i]]] = adj;
	}
	ptr.reset(new Chunk{loc, neighbors});
	return ptr.get();
}

bool World::ChunkExists(glm::ivec3 loc)
{
	return chunks_.find(loc) != chunks_.end();
}

void World::Render(const gl::Shader& block_shader, const control::Camera& camera)
{
	const auto frustum_aabb = camera.ComputeFrustumAABB();
	for (const auto& [location, chunk] : chunks_)
	{
		if (util::aabb_intersects(chunk->GetAABB(), frustum_aabb))
			chunk->Draw(block_shader);
	}
}

std::optional<Block> World::GetBlockAt(glm::ivec3 world_pos, bool create_if_not_exists)
{
	const auto chunk_pos = Chunk::block_to_chunk_pos(world_pos);
	if (!create_if_not_exists && !ChunkExists(chunk_pos))
		return std::nullopt;

	const glm::ivec3 loc_pos = Chunk::block_to_loc_pos(world_pos);
	return GetChunkAt(chunk_pos)->GetBlockAt(loc_pos);
}

Block* World::GetBlockRefAt(glm::ivec3 world_pos, bool create_if_not_exists, bool taint)
{
	const auto chunk_pos = Chunk::block_to_chunk_pos(world_pos);
	const glm::ivec3 loc_pos = Chunk::block_to_loc_pos(world_pos);

	Chunk* chunk = GetChunkAt(chunk_pos, create_if_not_exists);
	if (!chunk)
		return nullptr;
	return &chunk->GetBlockRefAt(loc_pos, taint);
}

void World::Update()
{
	int count = 0;
	for (const auto& cnk : chunks_)
	{
		if (cnk.second->UpdateMesh())
			count++;
		if (count >= kMaximumChunksPerUpdate)
			break;
	}
	if (count > 0)
		spdlog::debug("Updated {} chunks on render tick", count);
}
}
