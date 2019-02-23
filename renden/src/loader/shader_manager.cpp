//
// Created by netdex on 2/17/19.
//

#include <memory>

#include <loader/shader_manager.hpp>

namespace shaders {
    namespace block {

        std::weak_ptr<gl::shader> shader;

        std::vector<gl::varray_attribute> attribs;

        std::shared_ptr<gl::shader> load() {
            auto shr_shader = std::make_shared<gl::shader>();
            shader = shr_shader;
            shr_shader->attach("block.vert")
                    .attach("block.frag")
                    .link();

            attribs = std::vector<gl::varray_attribute>{
                    {shr_shader->get_attribute("position"), 3, gl::FLOAT, 6 * sizeof(float), 0,                 false, 0},
                    {shr_shader->get_attribute("texcoord"), 3, gl::FLOAT, 6 * sizeof(float), 3 *
                                                                                             sizeof(float),     false, 0}
            };
            // the loaded texture should never change
            shr_shader->bind("tex", 0);
            return shr_shader;
        }
    }

    namespace tenbox {
        std::weak_ptr<gl::shader> shader;
        std::vector<gl::varray_attribute> attribs;
        std::shared_ptr<skybox> tenbox;

        std::shared_ptr<gl::shader> load() {
            auto shr_shader = std::make_shared<gl::shader>();
            shader = shr_shader;
            shr_shader->attach("skybox.vert")
                    .attach("skybox.frag")
                    .link();

            attribs = std::vector<gl::varray_attribute>{
                    {shr_shader->get_attribute("position"), 3, gl::FLOAT, 3 * sizeof(float), 0, false, 0},
            };
            // the loaded texture should never change
            shr_shader->bind("tex", 0);

            tenbox = std::make_shared<skybox>(std::vector<std::string>{
                    PROJECT_SOURCE_DIR "/renden/res/skybox/alps_rt.tga",
                    PROJECT_SOURCE_DIR "/renden/res/skybox/alps_lf.tga",
                    PROJECT_SOURCE_DIR "/renden/res/skybox/alps_up.tga",
                    PROJECT_SOURCE_DIR "/renden/res/skybox/alps_dn.tga",
                    PROJECT_SOURCE_DIR "/renden/res/skybox/alps_bk.tga",
                    PROJECT_SOURCE_DIR "/renden/res/skybox/alps_ft.tga",
            });
            return shr_shader;
        }
    }

    std::vector<std::shared_ptr<gl::shader>> load() {
        auto block_shader = block::load();
        auto skybox_shader = tenbox::load();
        return {block_shader, skybox_shader};
    }


}