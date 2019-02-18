//
// Created by netdex on 2/17/19.
//

#ifndef RENDEN_BLOCK_TEXTURE_MANAGER_HPP
#define RENDEN_BLOCK_TEXTURE_MANAGER_HPP


#include <string>
#include <unordered_map>
#include <gl/texture.hpp>
#include <memory>
#include <primitive/block_primitive.hpp>

class block_manager {
    // convert name of texture to index of 2D texture array
    std::unordered_map<std::string, unsigned int> texture_name_to_layer;
    std::shared_ptr<gl::texture2d> textures;

    // convert name of block to block primitive mesh
    std::unordered_map<unsigned int, std::shared_ptr<block_primitive>> block_id_to_primitive;
    std::unordered_map<std::string, unsigned int> block_name_to_id;

    void load_textures(const std::string &block_tex_conf);

    void create_block_primitives(const std::string &block_def_conf);

public:
    block_manager(const std::string &block_tex_conf, const std::string &block_def_conf);

    std::shared_ptr<block_primitive> get_block_by_name(const std::string &name);
    std::shared_ptr<block_primitive> get_block_by_id(unsigned int id);
};

namespace world {
    namespace entities {
        namespace blocks {
            extern std::unique_ptr<block_manager> db;

            void load();
        }
    }
}

#endif //RENDEN_BLOCK_TEXTURE_MANAGER_HPP
