//
// Created by netdex on 2/17/19.
//

#ifndef RENDEN_BLOCK_PRIMITIVE_HPP
#define RENDEN_BLOCK_PRIMITIVE_HPP


#include <vector>
#include <glm/vec3.hpp>
#include <array>
#include <glad/glad.h>
#include <util/bytebuffer.hpp>

enum block_face : int {
    NEG_Z = 1<<0,
    POS_Z = 1<<1,
    NEG_X = 1<<2,
    POS_X = 1<<3,
    NEG_Y = 1<<4,
    POS_Y = 1<<5
};
typedef int block_face_set;

extern const block_face FACE_IDX_TO_BLOCK_FACE[];
extern const int BLOCK_FACE_TO_IDX[];
extern const glm::ivec3 FACE_IDX_TO_OFFSET[];

class block_primitive {
public:
	std::array<float, 6 * 4*3> uv;
    bool is_opaque;

    explicit block_primitive(std::array<float, 6 * 4*3> uv, bool is_opaque);

    void append_vertex_list(bytebuf<> &vlist, glm::ivec3 position, block_face_set faces);

};


#endif //RENDEN_BLOCK_PRIMITIVE_HPP
