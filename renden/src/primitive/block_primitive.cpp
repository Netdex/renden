//
// Created by netdex on 2/17/19.
//

#include <primitive/block_primitive.hpp>
#include <loader/shader_manager.hpp>

const block_face FACE_IDX_TO_BLOCK_FACE[] = {NEG_Z, POS_Z, NEG_X, POS_X, NEG_Y, POS_Y};

const int BLOCK_FACE_TO_IDX[] = {-1, 0, 1, -1, 2, -1, -1, -1,
                                 3, -1, -1, -1, -1, -1, -1, -1,
                                 4, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, 5}; // I hate this
const glm::ivec3 FACE_IDX_TO_OFFSET[] = {glm::ivec3(0, 0, -1), glm::ivec3(0, 0, 1), glm::ivec3(-1, 0, 0),
                                         glm::ivec3(1, 0, 0), glm::ivec3(0, -1, 0), glm::ivec3(0, 1, 0)};

block_primitive::block_primitive(std::array<float, 6 * 4 * 3> uv, bool is_opaque) : uv(uv), is_opaque(is_opaque) {

}


void block_primitive::append_vertex_list(bytebuf<> &vlist, glm::ivec3 position, block_face_set faces) {
    for (int i = 0; i < 6; i++) {
        if (faces & FACE_IDX_TO_BLOCK_FACE[i]) {
            vlist.put(glm::value_ptr(position), 3);
            vlist.put(i);
            vlist.put(uv.data() + i * 4 * 3, 4 * 3);
        }
    }
}




