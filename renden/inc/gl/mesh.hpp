#include <utility>

//
// Created by netdex on 2/15/19.
//

#ifndef RENDEN_MESH_HPP
#define RENDEN_MESH_HPP

#include <memory>
#include <glm/glm.hpp>

#include "buffer.hpp"
#include "varray.hpp"
#include "shader.hpp"

namespace gl {
    enum mesh_draw_mode : GLenum {
        TRIANGLES = GL_TRIANGLES,
        TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
        LINE_STRIP = GL_LINE_STRIP,
        QUADS = GL_QUADS
    };

    class mesh {
        std::shared_ptr<gl::varray> vao;
        std::shared_ptr<gl::buffer<gl::VERTEX_BUFFER, gl::STATIC_DRAW, float>> vbo;
        std::shared_ptr<gl::buffer<gl::ELEMENT_BUFFER, gl::STATIC_DRAW, GLuint>> ebo;
    public:


        glm::mat4 model = glm::mat4(1.f);

        mesh_draw_mode draw_mode;

        mesh(const mesh &o) = delete;


        mesh(const float *vertices, size_t vert_size, const GLuint *elements, size_t elem_size,
             mesh_draw_mode draw_mode,
             const varray_attribute *attributes, size_t attrib_count)
                : vao(std::make_shared<gl::varray>()),
                  vbo(std::make_shared<gl::buffer<gl::VERTEX_BUFFER, gl::STATIC_DRAW, float>>(vertices, vert_size)),
                  ebo(std::make_shared<gl::buffer<gl::ELEMENT_BUFFER, gl::STATIC_DRAW, GLuint>>(elements,
                                                                                                elem_size)),
                  draw_mode(draw_mode) {

            for (int i = 0; i < attrib_count; i++) {
                const varray_attribute &a = attributes[i];
                vao->set_attrib(a.location, a.size, a.type, a.stride, a.offset, a.normalized);
            }

            this->unbind();
        }

        mesh(const std::vector<float> &vertices, const std::vector<GLuint> &elements,
             mesh_draw_mode draw_mode, const std::vector<varray_attribute> &attributes)
                : mesh(&vertices[0], vertices.size() * sizeof(float), &elements[0], elements.size() * sizeof(GLuint),
                       draw_mode, &attributes[0], attributes.size()) {}

        mesh(const float *vertices, size_t vert_size,
             mesh_draw_mode draw_mode,
             const varray_attribute *attributes, size_t attrib_count)
                : vao(std::make_shared<gl::varray>()),
                  vbo(std::make_shared<gl::buffer<gl::VERTEX_BUFFER, gl::STATIC_DRAW, float>>(vertices, vert_size)),
                  draw_mode(draw_mode) {
            for (int i = 0; i < attrib_count; i++) {
                const varray_attribute &a = attributes[i];
                vao->set_attrib(a.location, a.size, a.type, a.stride, a.offset, a.normalized);
            }

            this->unbind();
        }

        mesh(const std::vector<float> &vertices,
             mesh_draw_mode draw_mode, const std::vector<varray_attribute> &attributes)
                : mesh(&vertices[0], vertices.size() * sizeof(float),
                       draw_mode, &attributes[0], attributes.size()) {}

        mesh(mesh_draw_mode draw_mode, const std::vector<varray_attribute> &attributes)
                : mesh(nullptr, 0, draw_mode, &attributes[0], attributes.size()) {

        }

        ~mesh() {
        }

        mesh operator=(const mesh &o) = delete;

        void bind() {
            vao->bind();
        }

        void unbind() {
            vao->unbind();
        }

        void draw_elements(const gl::shader &shader) {
            assert(ebo);
            this->bind();
            shader.bind("model", model);
            glDrawElements(static_cast<GLenum>(draw_mode),
                           static_cast<GLsizei>(ebo->get_elements()), GL_UNSIGNED_INT, 0);
            this->unbind();
        }

        void draw(const gl::shader &shader) {
            this->bind();
            shader.bind("model", model);
            glDrawArrays(static_cast<GLenum>(draw_mode), 0, static_cast<GLsizei>(vbo->get_elements()));
            this->unbind();
        }

        void buffer_vertex_data(const float *data, size_t size) {
            this->vbo->allocate(data, size);
//            this->vbo->bind();
//            for (int i = 0; i < attrib_count; i++) {
//                const varray_attribute &a = attributes[i];
//                vao->set_attrib(a.location, a.size, a.type, a.stride, a.offset, a.normalized);
//            }
//            this->vbo->unbind();
        }

        void buffer_vertex_data(const std::vector<float> &data) {
            this->buffer_vertex_data(data.data(), data.size() * sizeof(float));
        }
    };
}


#endif //RENDEN_MESH_HPP
