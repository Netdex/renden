//
// Created by netdex on 2/17/19.
//

#include <loader/block_manager.hpp>

#include "loader/block_manager.hpp"
#include <nlohmann/json.hpp>
#include <fstream>

block_manager::block_manager(const std::string &block_tex_conf, const std::string &block_def_conf) {
    this->load_textures(block_tex_conf);
    this->create_block_primitives(block_def_conf);
    this->create_block_buffer();
}

void block_manager::load_textures(const std::string &block_tex_conf) {
    std::ifstream block_tex_file(block_tex_conf);
    nlohmann::json j;
    block_tex_file >> j;

    int width = j["width"];
    int height = j["height"];

    std::vector<std::string> paths;
    unsigned int idx = 0;
    for (const auto &tex_def : j["textures"].items()) {
        const std::string &tex_name = tex_def.key();
        const std::string &tex_path = tex_def.value();
        texture_name_to_layer[tex_name] = idx;
        paths.push_back(PROJECT_SOURCE_DIR "/renden/res/tex/" + tex_path);
        ++idx;
    }

    textures = std::make_shared<gl::texture2d>(paths, width, height, gl::NEAREST, gl::CLAMP_EDGE, 0);
}

void block_manager::create_block_primitives(const std::string &block_def_conf) {
    std::ifstream block_def_file(block_def_conf);
    nlohmann::json j;
    block_def_file >> j;

    for (const auto &block_def : j) {
        unsigned int id = block_def["id"];
        std::string name = block_def["name"];
        std::vector<std::string> tex_names = block_def["textures"];
        std::array<float, 36 * 3> str = block_def["str"];
        bool is_opaque = block_def["opaque"];

        // assign proper r-layer for texture
        for (int i = 0; i < 36; i++) {
            str[i * 3 + 2] = texture_name_to_layer[tex_names[(int) str[i * 3 + 2]]];
        }

        auto new_block = std::make_shared<block_primitive>(str, is_opaque);
        block_id_to_primitive[id] = new_block;
        block_name_to_id[name] = id;
    }
}

void block_manager::create_block_buffer() {
    std::vector<float> vertex_data;
    unsigned int current_offset = 0;
    for (int id = 0; id < MAXIMUM_BLOCKS; id++) {
        auto prim = block_id_to_primitive[id];
        if (!prim) continue;
        for (int f = 0; f < 6; f++) {
            block_id_face_to_offset[id][f] = current_offset;
            current_offset += prim->append_vertex_list(vertex_data, glm::ivec3(), FACE_IDX_TO_BLOCK_FACE[f]);
        }
    }
    block_buffer = std::make_shared<gl::buffer<gl::VERTEX_BUFFER, gl::STATIC_DRAW, float>>(vertex_data);
}

std::shared_ptr<block_primitive> block_manager::get_block_by_name(const std::string &name) {
    if (block_name_to_id.find(name) != block_name_to_id.end())
        return block_id_to_primitive[block_name_to_id[name]];
    else return std::shared_ptr<block_primitive>(nullptr);
}

std::shared_ptr<block_primitive> block_manager::get_block_by_id(unsigned int id) {
    return block_id_to_primitive[id];
}


std::weak_ptr<block_manager> world::entities::blocks::db;

std::shared_ptr<block_manager> world::entities::blocks::load() {
    auto shr_db = std::make_shared<block_manager>(PROJECT_SOURCE_DIR "/renden/res/block_texture.json",
                                                  PROJECT_SOURCE_DIR "/renden/res/block_def.json");
    db = shr_db;
    return shr_db;
}
