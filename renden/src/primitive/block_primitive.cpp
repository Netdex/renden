//
// Created by netdex on 2/17/19.
//

#include <primitive/block_primitive.hpp>
#include <loader/shader_manager.hpp>

static const GLfloat CUBE_VERTICES[] = {
        // xy -z
        -0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        // xy +z
        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
        // yz -x
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        // yz +x
        0.5f, 0.5f, 0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        // xz -y
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f,
        // xz +y
        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, 0.5f,
};


block_primitive::block_primitive(float uv[36 * 3]) : gl::mesh(create_vertex_buffer(uv), gl::TRIANGLES,
                                          shaders::block::attribs) {

}

const std::vector<float> block_primitive::create_vertex_buffer(float uv[36 * 3]) {
    // merge base cube vertices with str coordinates
    std::vector<float> merged;
    for (int i = 0; i < 36; i++) {
        merged.insert(merged.end(), CUBE_VERTICES + (i * 3), CUBE_VERTICES + (i + 1) * 3);
        merged.insert(merged.end(), uv + (i * 3), uv + (i + 1) * 3);
    }
    return merged;
}

void block_primitive::draw() {
    assert(false); // TODO deprecated!
    gl::mesh::draw(*shaders::block::shader);
}
