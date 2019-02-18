//
// Created by netdex on 2/17/19.
//

#ifndef RENDEN_CHUNK_HPP
#define RENDEN_CHUNK_HPP


#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "block.hpp"

static const unsigned int CHUNK_WIDTH = 16;
static const unsigned int CHUNK_HEIGHT = 128;

class chunk {
public:
    // y-z-x order
    block data[CHUNK_HEIGHT][CHUNK_WIDTH][CHUNK_WIDTH];

    chunk() = default;

    void draw(const glm::mat4 &chunk);
};


#endif //RENDEN_CHUNK_HPP
