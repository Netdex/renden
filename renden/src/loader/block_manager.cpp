//
// Created by netdex on 2/17/19.
//

#include <loader/block_manager.hpp>

#include "loader/block_manager.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include "spdlog/spdlog.h"

block_manager::block_manager(const std::string &block_tex_conf, const std::string &block_def_conf) {
    this->load_textures(block_tex_conf);
    this->create_block_primitives(block_def_conf);
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
        assert(texture_name_to_layer.find(tex_name) == texture_name_to_layer.end());
        texture_name_to_layer[tex_name] = idx;
        paths.push_back(PROJECT_SOURCE_DIR "/renden/res/tex/" + tex_path);
        ++idx;
    }

    textures = std::make_shared<gl::texture2d>(paths, width, height, gl::NEAREST, gl::CLAMP_EDGE, 0);
    spdlog::debug("loaded {} textures", paths.size());
}

void block_manager::create_block_primitives(const std::string &block_def_conf) {
    std::ifstream block_def_file(block_def_conf);
    nlohmann::json j;
    block_def_file >> j;

    for (const auto &block_def : j) {
        unsigned int id = block_def["id"];
        std::string name = block_def["name"];
        std::vector<std::string> tex_names = block_def["textures"];
        std::array<float, 6 * 4 * 3> str = block_def["str"];
        bool is_opaque = block_def["opaque"];
        bool is_power_source = block_def["is_power_source"];
        bool is_conduit = block_def["is_conduit"];
        // assign proper r-layer for texture
        for (int i = 0; i < 4 * 6; i++) {
            str[i * 3 + 2] = texture_name_to_layer[tex_names[(int) str[i * 3 + 2]]];
        }

        auto new_block = std::make_shared<block_primitive>(str, is_opaque, is_power_source, is_conduit);

        assert(!block_id_to_primitive[id] && block_name_to_id.find(name) == block_name_to_id.end());
        block_id_to_primitive[id] = new_block;
        block_name_to_id[name] = id;
    }
    spdlog::debug("loaded {} block definitions", j.size());
}


std::shared_ptr<block_primitive> block_manager::get_block_by_name(const std::string &name) {
    auto result = block_name_to_id.find(name);
    return result != block_name_to_id.end() ? block_id_to_primitive[result->second] :
           std::shared_ptr<block_primitive>(nullptr);
}

std::shared_ptr<block_primitive> block_manager::get_block_by_id(unsigned int id) {
    return block_id_to_primitive[id];
}

std::optional<unsigned int> block_manager::get_block_id_by_name(const std::string &name) {
    auto result = block_name_to_id.find(name);
    return (result != block_name_to_id.end()) ? std::optional<unsigned int>{result->second} : std::nullopt;
}


std::weak_ptr<block_manager> world::entities::blocks::db;

std::shared_ptr<block_manager> world::entities::blocks::load() {
    auto shr_db = std::make_shared<block_manager>(PROJECT_SOURCE_DIR "/renden/res/block_texture.json",
                                                  PROJECT_SOURCE_DIR "/renden/res/block_def.json");
    db = shr_db;
    return shr_db;
}
