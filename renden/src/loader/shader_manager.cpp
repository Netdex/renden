//
// Created by netdex on 2/17/19.
//

#include <memory>

#include <loader/shader_manager.hpp>

namespace shaders {
    namespace block{

        std::unique_ptr<gl::shader> shader;

        std::vector<gl::varray_attribute> attribs;

        void load() {
            shader = std::make_unique<gl::shader>();
            shader->attach("block.vert")
                    .attach("block.frag")
                    .link();

            attribs = std::vector<gl::varray_attribute>{
                    {shader->get_attribute("position"), 3, gl::FLOAT, 6 * sizeof(float), 0,                 false},
                    {shader->get_attribute("texcoord"), 3, gl::FLOAT, 6 * sizeof(float), 3 * sizeof(float), false}
            };
            // the loaded texture should never change
            shader->bind("tex", 0);
        }
    }

    void load(){
        block::load();
    }


}