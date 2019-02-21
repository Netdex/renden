//
// Created by netdex on 2/17/19.
//

#include <world/chunk.hpp>
#include <loader/block_manager.hpp>
#include <loader/shader_manager.hpp>
#include <primitive/block_primitive.hpp>

#include "world/chunk.hpp"

chunk::chunk() : mesh(std::make_unique<gl::mesh>(gl::TRIANGLES, shaders::block::attribs)) {
    this->update_mesh();
}

void chunk::draw(const gl::shader &shader, const glm::mat4 &chunk) {
    // MUST be a block shader
    shader.bind("chunk", chunk);
    mesh->draw(shader);
}

void chunk::update_mesh() {
    std::vector<float> vlist;
    for (unsigned int y = 0; y < CHUNK_HEIGHT; y++) {
        for (unsigned int z = 0; z < CHUNK_WIDTH; z++) {
            for (unsigned int x = 0; x < CHUNK_WIDTH; x++) {
                std::shared_ptr<block_primitive> block_primitive =
                        world::entities::blocks::db.lock()->get_block_by_id(data[y][z][x].id);
                if (block_primitive) {
                    glm::ivec3 position = glm::ivec3(x, y, z);
                    block_primitive->append_vertex_list(vlist, position, visible_faces(position));
                }
            }
        }
    }
    mesh->buffer_vertex_data(vlist);
}


bool chunk::face_occluded(glm::ivec3 position, block_face face) {
    assert(position.x >= 0 && position.x < CHUNK_WIDTH
           && position.z >= 0 && position.z < CHUNK_WIDTH
           && position.y >= 0 && position.y < CHUNK_HEIGHT);
    // chunk boundaries can never be occluded
    if ((position.x == 0 && face == NEG_X) || (position.y == 0 && face == NEG_Y) ||
        (position.z == 0 && face == NEG_Z)
        || (position.x == CHUNK_WIDTH - 1 && face == POS_X)
        || (position.y == CHUNK_HEIGHT - 1 && face == POS_Y)
        || (position.z == CHUNK_WIDTH - 1 && face == POS_Z))
        return false;
    glm::ivec3 offset = FACE_IDX_TO_OFFSET[BLOCK_FACE_TO_IDX[face]] + position;
    if (offset.x < 0 || offset.x >= CHUNK_WIDTH || offset.y < 0 || offset.y >= CHUNK_HEIGHT
        || offset.z < 0 || offset.z >= CHUNK_WIDTH)
        return false;
    auto offset_block = world::entities::blocks::db.lock()->get_block_by_id(data[offset.y][offset.z][offset.x].id);
    return offset_block && offset_block->is_opaque;
}

block_face_set chunk::visible_faces(glm::ivec3 position) {
    block_face_set mask = 0;
    for (int i = 0; i < 6; i++) {
        block_face face = FACE_IDX_TO_BLOCK_FACE[i];
        if (!face_occluded(position, face))
            mask |= face;
    }
    return mask;
}


