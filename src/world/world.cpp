#include "world/world.hpp"

#include "util/math.hpp"

namespace world
{
Chunk* World::GetChunk(glm::ivec3 loc, bool create_if_not_exists)
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
		glm::ivec3 offset = ord_to_offset(i);
		Chunk* adj = GetChunk(loc + offset);
		neighbors[i] = adj;
		if (adj)
			adj->neighbors_[dir_to_ord(ord_to_dir_inv(i))] = adj;
	}
	ptr.reset(new Chunk{loc, neighbors});
	return ptr.get();
}

bool World::ChunkExists(glm::ivec3 loc)
{
	return chunks_.find(loc) != chunks_.end();
}

void World::Render(const gl::Shader& block_shader, const control::Camera& camera, bool frustum_cull)
{
	const auto frustum_aabb = camera.ComputeFrustumAABB();
	for (const auto& [location, chunk] : chunks_)
	{
		if (!frustum_cull || util::aabb_intersects(chunk->GetAABB(), frustum_aabb))
			chunk->Draw(block_shader);
	}
}

const Block* World::GetBlock(glm::ivec3 world_pos)
{
	const auto chunk_pos = Chunk::world_to_chunk(world_pos);
	const glm::ivec3 loc_pos = Chunk::world_to_chunk_local(world_pos);
	const auto chunk = GetChunk(chunk_pos);
	if (!chunk)
		return nullptr;
	return &chunk->GetBlock(loc_pos);
}

void World::SetBlock(glm::ivec3 world_pos, const Block& block)
{
	const auto chunk_pos = Chunk::world_to_chunk(world_pos);
	const glm::ivec3 loc_pos = Chunk::world_to_chunk_local(world_pos);
	GetChunk(chunk_pos, true)->SetBlock(loc_pos, block);
}

void World::Update()
{
	int count = 0;
	for (const auto& cnk : chunks_)
	{
		if (cnk.second->Update())
			count++;
		if (count >= kMaximumChunksPerUpdate)
			break;
	}
	if (count > 0)
		spdlog::debug("Updated {} chunks on render tick", count);
}
}
