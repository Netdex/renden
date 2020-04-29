#include "world/chunk.hpp"

namespace world
{
void Chunk::Draw(const gl::Shader& shader) const
{
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

bool Chunk::face_occluded(glm::ivec3 position, Direction face) const
{
	assert(position.x >= 0 && position.x < kChunkWidth
		&& position.z >= 0 && position.z < kChunkWidth
		&& position.y >= 0 && position.y < kChunkWidth);
	const glm::ivec3 offset = dir_to_offset(face) + position;
	if (offset.x < 0 || offset.x >= kChunkWidth || offset.y < 0 || offset.y >= kChunkWidth
		|| offset.z < 0 || offset.z >= kChunkWidth)
		return false;
	const auto& offset_block = data_[offset.y][offset.x][offset.z];
	return offset_block.id() && offset_block.table()["opaque"].as_boolean();
}

DirectionMask Chunk::visible_faces(glm::ivec3 position) const
{
	DirectionMask mask = 0;
	for (Direction face : kDirections)
	{
		if (!face_occluded(position, face))
			mask |= face;
	}
	return mask;
}
}
