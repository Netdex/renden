#include "world/chunk.hpp"

namespace world
{
void Chunk::Draw(const gl::Shader& shader) const
{
	const glm::vec3 translation = glm::vec3{chunk_to_block_pos(location_, glm::ivec3{})};
	glm::mat4 chunk = translate(glm::mat4(1.f), translation);
	shader.Bind("chunk", chunk);
	mesh_->Draw(shader);
}

std::pair<glm::vec3, glm::vec3> Chunk::GetAABB() const
{
	glm::ivec3 min = chunk_to_block_pos(location_, glm::ivec3{});
	return {glm::vec3{min}, glm::vec3{min} + glm::vec3{kChunkWidth}};
}

bool Chunk::UpdateMesh()
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
					const glm::ivec3 position = glm::ivec3(x, y, z);
					data_[y][x][z].AppendToVertexList(vlist, position, visible_faces(position));
				}
			}
		}
		mesh_->BufferVertexData(vlist);
		dirty_ = false;
		return true;
	}
	return false;
}

const Block& Chunk::GetBlockAt(glm::ivec3 loc) const
{
	assert(
		loc.x >= 0 && loc.x < kChunkWidth&& loc.y >= 0 && loc.y < kChunkWidth&& loc.z >= 0 && loc.z <
		kChunkWidth);
	return data_[loc.y][loc.x][loc.z];
}

Block& Chunk::GetBlockRefAt(glm::ivec3 loc, bool taint)
{
	assert(
		loc.x >= 0 && loc.x < kChunkWidth&& loc.y >= 0 && loc.y < kChunkWidth&& loc.z >= 0 && loc.z <
		kChunkWidth);
	dirty_ |= taint;
	return data_[loc.y][loc.x][loc.z];
}

bool Chunk::face_occluded(glm::ivec3 position, BlockFace face) const
{
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
	// TODO
	//const auto offset_block = Context<BlockManager>::Get().GetBlockById(data_[offset.y][offset.x][offset.z].id_);
	//return offset_block && offset_block->IsOpaque;
	return data_[offset.y][offset.x][offset.z].id();
}

block_face_mask_t Chunk::visible_faces(glm::ivec3 position) const
{
	block_face_mask_t mask = 0;
	for (BlockFace face : kFaceToBlock)
	{
		if (!face_occluded(position, face))
			mask |= face;
	}
	return mask;
}
}
