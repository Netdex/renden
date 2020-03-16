#ifndef RENDEN_CHUNK_HPP
#define RENDEN_CHUNK_HPP

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "block.hpp"
#include "control/camera.hpp"
#include "gl/mesh.hpp"
#include "loader/block_manager.hpp"
#include "loader/shader_program.hpp"
#include "primitive/block_primitive.hpp"
#include "util/context.hpp"

namespace world
{
constexpr int CHUNK_W = 16;
constexpr int CHUNK_H = 128;

class Chunk
{
	glm::ivec2 location_;
	std::unique_ptr<gl::Mesh<GLbyte>> mesh_;

	// y-z-x order
	Block data_[CHUNK_H][CHUNK_W][CHUNK_W];
	bool dirty_ = false;
public:

	Chunk(glm::ivec2 location) : location_(location),
	                             mesh_(std::make_unique<gl::Mesh<GLbyte>>(
		                             Context<shader::BlockShader>::Get().MeshAttributes, gl::POINTS))
	{
		//this->update_mesh();
	}

	void Draw(const gl::Shader& shader) const
	{
		if (is_visible())
		{
			glm::mat4 chunk = translate(glm::mat4(1.f), glm::vec3{chunk_to_block_pos(location_, glm::ivec3{})});
			// MUST be a block shader
			shader.Bind("chunk", chunk);
			mesh_->Draw(shader);
		}
	}

	void UpdateMesh()
	{
		// TODO Maybe use a thread pool for this?
		if (dirty_)
		{
			util::byte_buffer<> vlist;
			for (int y = 0; y < CHUNK_H; y++)
			{
				for (int z = 0; z < CHUNK_W; z++)
				{
					for (int x = 0; x < CHUNK_W; x++)
					{
						std::shared_ptr<BlockPrimitive> block_primitive =
							Context<BlockManager>::Get().GetBlockById(data_[y][x][z].Id);
						if (block_primitive)
						{
							const glm::ivec3 position = glm::ivec3(x, y, z);
							block_primitive->AppendToVertexList(vlist, position, visible_faces(position));
						}
					}
				}
			}
			mesh_->BufferVertexData(vlist);
			spdlog::debug("Regenerating chunk mesh");
			dirty_ = false;
		}
	}

	std::optional<Block> GetBlockAt(int x, int y, int z)
	{
		if (x >= 0 && x < CHUNK_W && y >= 0 && y < CHUNK_H && z >= 0 && z < CHUNK_W)
			return data_[y][x][z];
		return std::nullopt;
	}

	std::optional<std::reference_wrapper<Block>> GetBlockRefAt(int x, int y, int z,
	                                                           bool taint = true)
	{
		if (x >= 0 && x < CHUNK_W && y >= 0 && y < CHUNK_H && z >= 0 && z < CHUNK_W)
		{
			dirty_ |= taint;
			return data_[y][x][z];
		}
		return std::nullopt;
	}

	static glm::ivec2 block_to_chunk_pos(glm::ivec3 pos)
	{
		return glm::ivec2(int(floorf(float(pos.x) / CHUNK_W)), int(floorf(float(pos.z) / CHUNK_W)));
	}

	static glm::ivec3 chunk_to_block_pos(glm::ivec2 chunk_pos, glm::ivec3 loc)
	{
		return glm::ivec3(chunk_pos.x * CHUNK_W + loc.x, loc.y, chunk_pos.y * CHUNK_W + loc.z);
	}

	static glm::ivec3 block_to_loc_pos(glm::ivec3 pos)
	{
		// took me a few times to get this right
		return {(pos.x % CHUNK_W + CHUNK_W) % CHUNK_W, pos.y, (pos.z % CHUNK_W + CHUNK_W) % CHUNK_W};
	}

private:
	bool face_occluded(glm::ivec3 position, world::block::BlockFace face) const
	{
		using namespace world::block;

		assert(position.x >= 0 && position.x < CHUNK_W
			&& position.z >= 0 && position.z < CHUNK_W
			&& position.y >= 0 && position.y < CHUNK_H);
		// chunk boundaries can never be occluded
		if ((position.x == 0 && face == NEG_X) || (position.y == 0 && face == NEG_Y) ||
			(position.z == 0 && face == NEG_Z)
			|| (position.x == CHUNK_W - 1 && face == POS_X)
			|| (position.y == CHUNK_H - 1 && face == POS_Y)
			|| (position.z == CHUNK_W - 1 && face == POS_Z))
			return false;
		glm::ivec3 offset = kFaceToOffset[kFaceToIndex[face]] + position;
		if (offset.x < 0 || offset.x >= CHUNK_W || offset.y < 0 || offset.y >= CHUNK_H
			|| offset.z < 0 || offset.z >= CHUNK_W)
			return false;
		const auto offset_block = Context<BlockManager>::Get().GetBlockById(data_[offset.y][offset.x][offset.z].Id);
		return offset_block && offset_block->IsOpaque;
	}

	block::BlockFaceSet visible_faces(glm::ivec3 position) const
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

	bool is_visible() const
	{
		const control::Camera& camera = Context<control::Camera>::Get();
		glm::vec3 pos = camera.Position;
		glm::vec3 dir = camera.GetDirection();
		// TODO Frustum culling
		return true;
	}
};
}


#endif //RENDEN_CHUNK_HPP
