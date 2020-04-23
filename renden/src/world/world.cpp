#include "world/world.hpp"

#include "util/math.hpp"

bool world::World::ivec3_map_predicate::operator()(const glm::ivec3& a, const glm::ivec3& b) const
{
	if (a.x > b.x) return false;
	if (a.x == b.x)
	{
		if (a.y > b.y) return false;
		if (a.y == b.y)
		{
			if (a.z >= b.z) return false;
		}
	}
	return true;
}

world::Chunk* world::World::GetChunkAt(glm::ivec3 loc, bool create_if_not_exists)
{
	// if chunk does not exist, it will be default constructed
	auto& ptr = chunks_[loc];
	if (!ptr && create_if_not_exists)
	{
		ptr.reset(new Chunk{loc});
	}
	return ptr.get();
}

bool world::World::ChunkExists(glm::ivec3 loc)
{
	return chunks_.find(loc) != chunks_.end();
}

void world::World::Render(const gl::Shader& block_shader, const control::Camera &camera)
{
	const auto frustum_aabb = camera.ComputeFrustumAABB();
	for (const auto& [location, chunk] : chunks_)
	{
		if(util::aabb_intersects(chunk->GetAABB(), frustum_aabb))
			chunk->Draw(block_shader);
	}
}

std::optional<world::Block> world::World::GetBlockAt(glm::ivec3 world_pos, bool create_if_not_exists)
{
	const auto chunk_pos = Chunk::block_to_chunk_pos(world_pos);
	if (!create_if_not_exists && !ChunkExists(chunk_pos))
		return std::nullopt;

	const glm::ivec3 loc_pos = Chunk::block_to_loc_pos(world_pos);
	return GetChunkAt(chunk_pos)->GetBlockAt(loc_pos);
}

world::Block* world::World::GetBlockRefAt(glm::ivec3 world_pos, bool create_if_not_exists, bool taint)
{
	const auto chunk_pos = Chunk::block_to_chunk_pos(world_pos);
	const glm::ivec3 loc_pos = Chunk::block_to_loc_pos(world_pos);

	Chunk* chunk = GetChunkAt(chunk_pos, create_if_not_exists);
	if (!chunk)
		return nullptr;
	return &chunk->GetBlockRefAt(loc_pos, taint);
}

void world::World::Update()
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
