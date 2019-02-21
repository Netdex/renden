//
// Created by netdex on 2019-02-18.
//

#ifndef RENDEN_SKYBOX_HPP
#define RENDEN_SKYBOX_HPP


#include <gl/texture.hpp>
#include <gl/mesh.hpp>

class skybox {
    std::shared_ptr<gl::cubemap> texture;
    std::shared_ptr<gl::mesh> mesh;
public:
    skybox(const std::vector<std::string> &paths);

    void draw(const gl::shader &shader);
};


#endif //RENDEN_SKYBOX_HPP
