//
// Created by netdex on 2/17/19.
//

#ifndef RENDEN_CHUNK_HPP
#define RENDEN_CHUNK_HPP


#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <gl/mesh.hpp>
#include <primitive/block_primitive.hpp>
#include "block.hpp"
#include <loader/block_manager.hpp>
#include <loader/shader_manager.hpp>

template<unsigned int W, unsigned int H>
class chunk {
	std::unique_ptr<gl::mesh<GLbyte>> mesh;

	std::shared_ptr<block_manager> cached_block_mgr;

	// y-z-x order
	block data[H][W][W];
	bool dirty = false;
public:


	chunk() : mesh(std::make_unique<gl::mesh<GLbyte>>(gl::POINTS, shaders::block::attribs)) {
		//this->update_mesh();
		cached_block_mgr = world::entities::blocks::db.lock();
	}

	void draw(const gl::shader &shader, const glm::mat4 &chunk) {
		// MUST be a block shader
		shader.bind("chunk", chunk);
		mesh->draw(shader);
	}
	void update_mesh() {
		if (dirty) {
		    bytebuf<> vlist;
			for (unsigned int y = 0; y < H; y++) {
				for (unsigned int z = 0; z < W; z++) {
					for (unsigned int x = 0; x < W; x++) {
						std::shared_ptr<block_primitive> block_primitive =
							cached_block_mgr->get_block_by_id(data[y][x][z].id);
						if (block_primitive) {
							glm::ivec3 position = glm::ivec3(x, y, z);
							block_primitive->append_vertex_list(vlist, position, visible_faces(position));
						}
					}
				}
			}
//			fprintf(stderr, "%d\n", vlist.size());
			mesh->buffer_vertex_data(vlist.data(), vlist.size());
			spdlog::debug("regenerating chunk mesh");
			dirty = false;
		}
	}

	std::optional<block> get_block_at(unsigned int x, unsigned int y, unsigned int z) {
		if (x >= 0 && x < W && y >= 0 && y < H && z >= 0 && z < W)
			return data[y][x][z];
		return {};
	}

	std::optional<std::reference_wrapper<block>> get_block_ref_at(unsigned int x, unsigned int y, unsigned int z,
			bool taint = true) {
		if (x >= 0 && x < W && y >= 0 && y < H && z >= 0 && z < W) {
			dirty |= taint;
			return data[y][x][z];
		}
		return {};
	}
private:
	bool face_occluded(glm::ivec3 position, block_face face) {
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
		glm::ivec3 offset = FACE_IDX_TO_OFFSET[BLOCK_FACE_TO_IDX[face]] + position;
		if (offset.x < 0 || offset.x >= W || offset.y < 0 || offset.y >= H
			|| offset.z < 0 || offset.z >= W)
			return false;
		auto offset_block = cached_block_mgr->get_block_by_id(data[offset.y][offset.x][offset.z].id);
		return offset_block && offset_block->is_opaque;
	}

	block_face_set visible_faces(glm::ivec3 position) {
		block_face_set mask = 0;
		for (int i = 0; i < 6; i++) {
			block_face face = FACE_IDX_TO_BLOCK_FACE[i];
			if (!face_occluded(position, face))
				mask |= face;
		}
		return mask;
	}


};


#endif //RENDEN_CHUNK_HPP
