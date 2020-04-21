#include "world/chunk.hpp"

void world::Chunk::Draw(const gl::Shader& shader) const
{
	const glm::vec3 translation = glm::vec3{chunk_to_block_pos(location_, glm::ivec3{})};
	glm::mat4 chunk = translate(glm::mat4(1.f), translation);
	shader.Bind("chunk", chunk);
	mesh_->Draw(shader);
}

std::pair<glm::vec3, glm::vec3> world::Chunk::GetAABB() const
{
	glm::ivec3 min = chunk_to_block_pos(location_, glm::ivec3{});
	return { glm::vec3{min}, glm::vec3{min} + glm::vec3{kChunkWidth} };
}

bool world::Chunk::UpdateMesh()
{
	// TODO Maybe use a thread pool for this?
	if (dirty_)
	{
		util::byte_buffer<> vlist;
		for (int y = 0; y < kChunkWidth; y++)
		{
			for (int z = 0; z < kChunkWidth; z++)
			{
				for (int x = 0; x < kChunkWidth; x++)
				{
					std::shared_ptr<BlockPrimitive> block_primitive =
						Context<BlockManager>::Get().GetBlockById(data_[y][x][z].ID);
					if (block_primitive)
					{
						const glm::ivec3 position = glm::ivec3(x, y, z);
						block_primitive->AppendToVertexList(vlist, position, visible_faces(position));
					}
				}
			}
		}
		mesh_->BufferVertexData(vlist);
		dirty_ = false;
		return true;
	}
	return false;
}

Block world::Chunk::GetBlockAt(glm::ivec3 loc)
{
	return GetBlockRefAt(loc, false);
}

Block& world::Chunk::GetBlockRefAt(glm::ivec3 loc, bool taint)
{
	assert(
		loc.x >= 0 && loc.x < kChunkWidth && loc.y >= 0 && loc.y < kChunkWidth && loc.z >= 0 && loc.z <
		kChunkWidth);
	dirty_ |= taint;
	return data_[loc.y][loc.x][loc.z];
}

bool world::Chunk::face_occluded(glm::ivec3 position, world::block::BlockFace face) const
{
	using namespace world::block;

	assert(position.x >= 0 && position.x < kChunkWidth
		&& position.z >= 0 && position.z < kChunkWidth
		&& position.y >= 0 && position.y < kChunkWidth);
	// chunk boundaries can never be occluded
	if ((position.x == 0 && face == NEG_X) || (position.y == 0 && face == NEG_Y) ||
		(position.z == 0 && face == NEG_Z)
		|| (position.x == kChunkWidth - 1 && face == POS_X)
		|| (position.y == kChunkWidth - 1 && face == POS_Y)
		|| (position.z == kChunkWidth - 1 && face == POS_Z))
		return false;
	const glm::ivec3 offset = kFaceToOffset[kFaceToIndex[face]] + position;
	if (offset.x < 0 || offset.x >= kChunkWidth || offset.y < 0 || offset.y >= kChunkWidth
		|| offset.z < 0 || offset.z >= kChunkWidth)
		return false;
	const auto offset_block = Context<BlockManager>::Get().GetBlockById(data_[offset.y][offset.x][offset.z].ID);
	return offset_block && offset_block->IsOpaque;
}

world::block::BlockFaceSet world::Chunk::visible_faces(glm::ivec3 position) const
{
	using namespace world::block;

	BlockFaceSet mask = 0;
	for (BlockFace face : kFaceToBlock)
	{
		if (!face_occluded(position, face))
			mask |= face;
	}
	return mask;
}
