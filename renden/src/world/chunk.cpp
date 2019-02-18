//
// Created by netdex on 2/17/19.
//

#include <world/chunk.hpp>
#include <loader/block_manager.hpp>
#include <loader/shader_manager.hpp>
#include <primitive/block_primitive.hpp>

#include "world/chunk.hpp"

void chunk::draw(const glm::mat4 &chunk) {
    shaders::block::shader->bind("chunk", chunk);
    for (unsigned int y = 0; y < CHUNK_HEIGHT; y++) {
        for (unsigned int z = 0; z < CHUNK_WIDTH; z++) {
            for (unsigned int x = 0; x < CHUNK_WIDTH; x++) {
                std::shared_ptr<block_primitive> block_primitive =
                        world::entities::blocks::db->get_block_by_id(data[y][z][x].id);
                if (block_primitive) {
                    if(x > 0 && x < CHUNK_WIDTH - 1 &&
                    z > 0 && z < CHUNK_WIDTH - 1 &&
                    y > 0 && y < CHUNK_HEIGHT - 1){
                        if(data[y-1][z][x].id && data[y+1][z][x].id && data[y][z-1][x].id &&
                        data[y][z+1][x].id && data[y][z][x-1].id && data[y][z][x+1].id)
                            continue;
                    }
                    block_primitive->model = glm::translate(glm::mat4(1.f), glm::vec3(x, y, z));
                    block_primitive->draw();
                }
            }
        }
    }
}
