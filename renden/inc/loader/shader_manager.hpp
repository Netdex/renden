//
// Created by netdex on 2/17/19.
//

#ifndef RENDEN_SHADER_MANAGER_HPP
#define RENDEN_SHADER_MANAGER_HPP

#include <vector>


#include <gl/varray.hpp>
#include <gl/shader.hpp>

namespace shaders {
    namespace block {
        extern std::unique_ptr<gl::shader> shader;
        extern std::vector<gl::varray_attribute> attribs;

        void load();

    }

    void load();
}


#endif //RENDEN_SHADER_MANAGER_HPP
