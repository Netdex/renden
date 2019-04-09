//
// Created by netdex on 2/24/19.
//

#ifndef RENDEN_CHUNK_MANAGER_HPP
#define RENDEN_CHUNK_MANAGER_HPP


#include <unordered_map>
#include <map>
#include <optional>

#include <glm/vec2.hpp>
#include "spdlog/spdlog.h"

#include <world/chunk.hpp>

template<unsigned int W, unsigned int H>
class chunk_manager {

	std::map<std::pair<int, int>, std::shared_ptr<chunk<W, H>>> chunks;

public:
	chunk_manager() {}

	std::shared_ptr<chunk<W, H>> get_chunk_at(int x, int z) {
		// if chunk does not exist, it will be default constructed
		auto& ptr = chunks[std::make_pair(x, z)];
		if (!ptr) {
			spdlog::debug("generated chunk ({},{}) due to first time access", x, z);
			ptr = std::make_shared<chunk<W, H>>();
		}
		return ptr;
	}

	bool chunk_exists(int x, int z) {
		return chunks.find(std::make_pair(x, z)) != chunks.end();
	}

	void render(const gl::shader &block) {
		for (auto cnk : chunks) {
			cnk.second->draw(block, glm::translate(glm::mat4(1.f),
				glm::vec3(cnk.first.first * W, 0, cnk.first.second * W)));
		}
	}

	glm::ivec2 block_pos_to_chunk_pos(glm::ivec3 pos) {
		return glm::ivec2(pos.x / W, pos.z / W);
	}

	std::optional<block> get_block_at(glm::ivec3 world_pos) {
		glm::vec2 chunk_pos = block_pos_to_chunk_pos(world_pos);
		if (!chunk_exists(chunk_pos.x, chunk_pos.y))
			return std::nullopt;
		int loc_x = world_pos.x % W;
		int loc_z = world_pos.z % W;
		return get_chunk_at(chunk_pos.x, chunk_pos.y)->get_block_at(loc_x, world_pos.y, loc_z);
	}

	block* get_block_ref_at(glm::ivec3 world_pos) {
		glm::vec2 chunk_pos = block_pos_to_chunk_pos(world_pos);
		if (!chunk_exists(chunk_pos.x, chunk_pos.y))
			return nullptr;
		int loc_x = world_pos.x % W;
		int loc_z = world_pos.z % W;
		return &get_chunk_at(chunk_pos.x, chunk_pos.y)->get_block_ref_at(loc_x, world_pos.y, loc_z);
	}

	void update_all_meshes() {
		for (auto cnk : chunks) {
			cnk.second->update_mesh();
		}
	}
};

namespace world::chunk {
	const int CHUNK_W = 16;
	const int CHUNK_H = 128;

	typedef chunk_manager<CHUNK_W, CHUNK_H> chunk_mgr_t;

	extern std::weak_ptr<chunk_mgr_t> db;

	std::shared_ptr<chunk_mgr_t> load();
}

#endif //RENDEN_CHUNK_MANAGER_HPP
