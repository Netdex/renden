#include <utility>

//
// Created by netdex on 21/04/19.
//

#include <phy/phy_engine.hpp>

void phy_engine::run() {
    while (should_run) {
        tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

phy_engine::phy_engine() : thread{[this] { run(); }} {
}

phy_engine::~phy_engine() {
    should_run = false;
}

void phy_engine::tick() {
    using namespace world::block;

    auto chunk_mgr = world::chunk::db.lock();
    auto block_mgr = world::entities::blocks::db.lock();

    for (auto &pair : chunk_mgr->chunks) {
        std::pair<int, int> loc = pair.first;
        auto cnk = pair.second;

        block scratch[world::chunk::CHUNK_W][world::chunk::CHUNK_H][world::chunk::CHUNK_W];
        bool dirty = false;

        for (int z = 0; z < world::chunk::CHUNK_W; z++) {
            for (int y = 0; y < world::chunk::CHUNK_H; y++) {
                for (int x = 0; x < world::chunk::CHUNK_W; x++) {
                    scratch[z][y][x] = *cnk->get_block_at(x,y,z);
                }
            }
        }

        for (int z = 0; z < world::chunk::CHUNK_W; z++) {
            for (int y = 0; y < world::chunk::CHUNK_H; y++) {
                for (int x = 0; x < world::chunk::CHUNK_W; x++) {
                    auto block = *cnk->get_block_at(x, y, z);
                    auto block_def = block_mgr->get_block_by_id(block.id);

                    if (block_def) {
                        auto blockcopy = block;
                        if (block_def->is_conduit) {
                            unsigned char max_neighbor_pstate = 0;
                            for (int i = 0; i < 6; i++) {
                                glm::ivec3 offset = FACE_IDX_TO_OFFSET[i];
//                                auto neighbor = chunk_mgr->get_block_at(
//                                        chunk_mgr->chunk_pos_to_block_pos(loc, glm::ivec3{x,y,z}+offset));
//                                auto neighbor = cnk->get_block_at(x + offset.x, y + offset.y, z + offset.z);
                                std::optional<decltype(block)> neighbor = {};
                                if (neighbor /*&& block_mgr->get_block_by_id(neighbor->id)->is_conduit*/) {
                                    max_neighbor_pstate = std::max(max_neighbor_pstate, neighbor->get_power_state());
                                }
                            }
                            unsigned char block_pstate =
                                    std::max(max_neighbor_pstate - 1, 0);
                            blockcopy.set_passive_power_state(block_pstate);
                            if (block_pstate > 0) {
                                blockcopy.id = *block_mgr->get_block_id_by_name("powered_conduit");
                            } else {
                                blockcopy.id = *block_mgr->get_block_id_by_name("conduit");
                            }
                        } else {
                            blockcopy.set_passive_power_state(0);
                        }

                        dirty |= blockcopy.id != block.id;
                        scratch[z][y][x] = blockcopy;
                    }
                }
            }
        }
        for (int z = 0; z < world::chunk::CHUNK_W; z++) {
            for (int y = 0; y < world::chunk::CHUNK_H; y++) {
                for (int x = 0; x < world::chunk::CHUNK_W; x++) {
                    cnk->get_block_ref_at(x, y, z, dirty)->get() = scratch[z][y][x];
                }
            }
        }
    }
}

