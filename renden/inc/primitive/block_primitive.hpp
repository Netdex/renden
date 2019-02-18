//
// Created by netdex on 2/17/19.
//

#ifndef RENDEN_BLOCK_PRIMITIVE_HPP
#define RENDEN_BLOCK_PRIMITIVE_HPP


#include <gl/mesh.hpp>

class block_primitive : public gl::mesh {


    static const std::vector<float> create_vertex_buffer(float uv[24 * 3]);

public:
    explicit block_primitive(float uv[24 * 3]);

    void draw();
};


#endif //RENDEN_BLOCK_PRIMITIVE_HPP
