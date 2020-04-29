#include "world/chunk.hpp"

namespace world
{
void Chunk::Draw(const gl::Shader& shader) const
{
	mesh_->Draw(shader);
}

std::pair<glm::vec3, glm::vec3> Chunk::GetAABB() const
{
	glm::ivec3 min = chunk_to_world(location_, glm::ivec3{});
	return {glm::vec3{min}, glm::vec3{min} + glm::vec3{kChunkWidth}};
}

bool Chunk::Update()
{
	// TODO Maybe use a thread pool for this?
	if (IsDirty())
	{
		util::byte_buffer<> vlist;
		for (int y = 0; y < kChunkWidth; y++)
		{
			for (int x = 0; x < kChunkWidth; x++)
			{
				for (int z = 0; z < kChunkWidth; z++)
				{
					const glm::ivec3 position = glm::ivec3(x, y, z);
					data_[y][x][z].AppendToVertexList(vlist, position, visible_faces(position));
				}
			}
		}
		mesh_->BufferVertexData(vlist);
		SetDirty(false);
		return true;
	}
	return false;
}


const Block& Chunk::GetBlock(glm::ivec3 loc) const
{
	assert(in_chunk_bounds(loc));
	return data_[loc.y][loc.x][loc.z];
}

void Chunk::SetBlock(glm::ivec3 loc, const Block& block)
{
	assert(in_chunk_bounds(loc));
	data_[loc.y][loc.x][loc.z] = block;

	dirty_ = true;
}

bool Chunk::face_occluded(glm::ivec3 position, Direction face) const
{
	assert(in_chunk_bounds(position));
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
