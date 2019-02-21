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

static const unsigned int CHUNK_WIDTH = 16;
static const unsigned int CHUNK_HEIGHT = 128;

class chunk {
    std::unique_ptr<gl::mesh> mesh;
public:
    // y-z-x order
    block data[CHUNK_HEIGHT][CHUNK_WIDTH][CHUNK_WIDTH];

    chunk();

    void draw(const gl::shader &shader, const glm::mat4 &chunk);
    void update_mesh();

private:
    bool face_occluded(glm::ivec3 position, block_face face);
    block_face_set visible_faces(glm::ivec3 position);
};


#endif //RENDEN_CHUNK_HPP
