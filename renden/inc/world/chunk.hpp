#ifndef RENDEN_CHUNK_HPP
#define RENDEN_CHUNK_HPP

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "gl/mesh.hpp"
#include "primitive/block_primitive.hpp"
#include "block.hpp"
#include "loader/block_manager.hpp"
#include "loader/shader_manager.hpp"

template <int W, int H>
class Chunk
{
	static_assert(W > 0 && H > 0, "chunk must have positive W, H");

	std::unique_ptr<gl::Mesh<GLbyte>> mesh_;

	std::shared_ptr<BlockManager> cached_block_mgr_;

	// y-z-x order
	Block data_[H][W][W];
	bool dirty_ = false;
public:

	Chunk() : mesh_(std::make_unique<gl::Mesh<GLbyte>>(gl::POINTS, shaders::block::attribs))
	{
		//this->update_mesh();
		cached_block_mgr_ = world::block::db.lock();
	}

	void Draw(const gl::Shader& shader, const glm::mat4& chunk) const
	{
		// MUST be a block shader
		shader.Bind("chunk", chunk);
		mesh_->Draw(shader);
	}

	void UpdateMesh()
	{
		if (dirty_)
		{
			ByteBuffer<> vlist;
			for (int y = 0; y < H; y++)
			{
				for (int z = 0; z < W; z++)
				{
					for (int x = 0; x < W; x++)
					{
						std::shared_ptr<BlockPrimitive> block_primitive =
							cached_block_mgr_->GetBlockById(data_[y][x][z].Id);
						if (block_primitive)
						{
							const glm::ivec3 position = glm::ivec3(x, y, z);
							block_primitive->AppendToVertexList(vlist, position, visible_faces(position));
						}
					}
				}
			}
			//			fprintf(stderr, "%d\n", vlist.size());
			mesh_->BufferVertexData(vlist.data(), vlist.size());
			spdlog::debug("Regenerating chunk mesh");
			dirty_ = false;
		}
	}

	std::optional<Block> GetBlockAt(int x, int y, int z)
	{
		if (x >= 0 && x < W && y >= 0 && y < H && z >= 0 && z < W)
			return data_[y][x][z];
		return {};
	}

	std::optional<std::reference_wrapper<Block>> GetBlockRefAt(int x, int y, int z,
	                                                           bool taint = true)
	{
		if (x >= 0 && x < W && y >= 0 && y < H && z >= 0 && z < W)
		{
			dirty_ |= taint;
			return data_[y][x][z];
		}
		return {};
	}

private:
	bool face_occluded(glm::ivec3 position, world::block::BlockFace face)
	{
		using namespace world::block;

		assert(position.x >= 0 && position.x < W
			&& position.z >= 0 && position.z < W
			&& position.y >= 0 && position.y < H);
		// chunk boundaries can never be occluded
		if ((position.x == 0 && face == NEG_X) || (position.y == 0 && face == NEG_Y) ||
			(position.z == 0 && face == NEG_Z)
			|| (position.x == W - 1 && face == POS_X)
			|| (position.y == H - 1 && face == POS_Y)
			|| (position.z == W - 1 && face == POS_Z))
			return false;
		glm::ivec3 offset = kFaceToOffset[kFaceToIndex[face]] + position;
		if (offset.x < 0 || offset.x >= W || offset.y < 0 || offset.y >= H
			|| offset.z < 0 || offset.z >= W)
			return false;
		auto offset_block = cached_block_mgr_->GetBlockById(data_[offset.y][offset.x][offset.z].Id);
		return offset_block && offset_block->IsOpaque;
	}

	world::block::BlockFaceSet visible_faces(glm::ivec3 position)
	{
		using namespace world::block;

		BlockFaceSet mask = 0;
		for (int i = 0; i < 6; i++)
		{
			const BlockFace face = kFaceToBlock[i];
			if (!face_occluded(position, face))
				mask |= face;
		}
		return mask;
	}
};


#endif //RENDEN_CHUNK_HPP
