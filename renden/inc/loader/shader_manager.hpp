//
// Created by netdex on 2/17/19.
//

#ifndef RENDEN_SHADER_MANAGER_HPP
#define RENDEN_SHADER_MANAGER_HPP

#include <vector>


#include <gl/varray.hpp>
#include <gl/shader.hpp>
#include <world/skybox.hpp>

namespace shaders {
    namespace block {
        extern std::weak_ptr<gl::shader> shader;
        extern std::vector<gl::varray_attribute> attribs;

        std::shared_ptr<gl::shader> load();

    }
    namespace tenbox {
        extern std::weak_ptr<gl::shader> shader;
        extern std::vector<gl::varray_attribute> attribs;
        extern std::shared_ptr<skybox> tenbox;

        std::shared_ptr<gl::shader> load();
    }

    // we keep a reference so we can control when it's destroyed
    std::vector<std::shared_ptr<gl::shader>> load();
}


#endif //RENDEN_SHADER_MANAGER_HPP
