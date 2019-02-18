//
// Created by netdex on 2/16/19.
//

#ifndef RENDEN_TEXTURE_HPP
#define RENDEN_TEXTURE_HPP

#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

namespace gl {
    enum texture_wrap_mode : GLenum {
        REPEAT = GL_REPEAT,
        MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
        CLAMP_EDGE = GL_CLAMP_TO_EDGE,
        CLAMP_BORDER = GL_CLAMP_TO_BORDER
    };

    enum texture_wrap_dimension : GLenum {
        S = GL_TEXTURE_WRAP_S,
        T = GL_TEXTURE_WRAP_T,
        R = GL_TEXTURE_WRAP_R
    };

    enum texture_filter_mode : GLenum {
        NEAREST = GL_NEAREST,
        LINEAR = GL_LINEAR,
        NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
        LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
        NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
        LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
    };

    enum texture_filter_case : GLenum {
        SCALE_UP = GL_TEXTURE_MAG_FILTER,
        SCALE_DOWN = GL_TEXTURE_MIN_FILTER
    };

    class texture2d {
        GLuint id;
    public:
        GLuint tex_id;

        texture2d(const texture2d &o) = delete;

        texture2d(const std::string paths[], size_t count,
                  unsigned int width, unsigned int height,
                  texture_filter_mode filter_mode, texture_wrap_mode wrap_mode,
                  unsigned int tex_id) : tex_id(tex_id) {
            glGenTextures(1, &id);
            glActiveTexture(GL_TEXTURE0 + tex_id);
            this->bind();

            glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, static_cast<GLsizei>(count));

            for (int i = 0; i < count; i++) {
                const std::string &path = paths[i];
                int w, h, bpp;
                unsigned char *imgbuf = stbi_load(path.c_str(), &w, &h, &bpp, 4);
                assert(imgbuf != nullptr && width == w && height == h && bpp == 4);
                glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, w, h, 1,
                                GL_RGBA, GL_UNSIGNED_BYTE, imgbuf);
                stbi_image_free(imgbuf);
            }

            this->set_filter_mode(SCALE_UP, filter_mode);
            this->set_filter_mode(SCALE_DOWN, filter_mode);
            this->set_wrap_mode(S, wrap_mode);
            this->set_wrap_mode(T, wrap_mode);
            if (filter_mode == NEAREST_MIPMAP_NEAREST || filter_mode == LINEAR_MIPMAP_NEAREST
                || filter_mode == NEAREST_MIPMAP_LINEAR || filter_mode == LINEAR_MIPMAP_LINEAR) {
                glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
            }
        }

        texture2d(const std::vector<std::string> &paths, unsigned int width, unsigned int height,
                  texture_filter_mode filter_mode, texture_wrap_mode wrap_mode,
                  unsigned int tex_id) : texture2d(&paths[0], paths.size(), width, height,
                                                   filter_mode, wrap_mode, tex_id) {}

        ~texture2d() {
            glDeleteTextures(1, &id);
        }

        texture2d operator=(const texture2d &o) = delete;

        void bind() {
            glBindTexture(GL_TEXTURE_2D_ARRAY, id);
        }

        void set_filter_mode(texture_filter_case cse, texture_filter_mode mode) {
            glTexParameteri(GL_TEXTURE_2D_ARRAY, static_cast<GLenum>(cse),
                            static_cast<GLenum>(mode));
        }

        void set_wrap_mode(texture_wrap_dimension dim, texture_wrap_mode mode) {
            glTexParameteri(GL_TEXTURE_2D_ARRAY, static_cast<GLenum>(dim),
                            static_cast<GLenum>(mode));
        }

        void set_border_color(glm::vec4 color) {
            glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color));
        }

    };

    class cubemap {
        GLuint id;
    public:
        GLuint tex_id;

        cubemap(const texture2d &o) = delete;

        cubemap(const std::string paths[6],
                texture_filter_mode filter_mode, texture_wrap_mode wrap_mode,
                unsigned int tex_id) : tex_id(tex_id) {
            glGenTextures(1, &id);
            glActiveTexture(GL_TEXTURE0 + tex_id);
            this->bind();

            for (int i = 0; i < 6; i++) {
                const std::string &path = paths[i];
                int w, h, bpp;
                unsigned char *imgbuf = stbi_load(path.c_str(), &w, &h, &bpp, 4);
                assert(imgbuf != nullptr && bpp == 4);
                glTexImage2D(static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i), 0, GL_RGBA, w, h, 0,
                             GL_RGBA, GL_UNSIGNED_BYTE, imgbuf);
                stbi_image_free(imgbuf);
            }

            this->set_filter_mode(SCALE_UP, filter_mode);
            this->set_filter_mode(SCALE_DOWN, filter_mode);
            this->set_wrap_mode(S, wrap_mode);
            this->set_wrap_mode(T, wrap_mode);
            this->set_wrap_mode(R, wrap_mode);

            if (filter_mode == NEAREST_MIPMAP_NEAREST || filter_mode == LINEAR_MIPMAP_NEAREST
                || filter_mode == NEAREST_MIPMAP_LINEAR || filter_mode == LINEAR_MIPMAP_LINEAR) {
                glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
            }
        }

        cubemap(const std::vector<std::string> &paths,
                texture_filter_mode filter_mode, texture_wrap_mode wrap_mode,
                unsigned int tex_id) : cubemap((assert(paths.size() == 6), &paths[0]),
                                               filter_mode, wrap_mode, tex_id) {}

        ~cubemap() {
            glDeleteTextures(1, &id);
        }

        texture2d operator=(const texture2d &o) = delete;

        void bind() {
            glBindTexture(GL_TEXTURE_CUBE_MAP, id);
        }

        void set_filter_mode(texture_filter_case cse, texture_filter_mode mode) {
            glTexParameteri(GL_TEXTURE_CUBE_MAP, static_cast<GLenum>(cse),
                            static_cast<GLenum>(mode));
        }

        void set_wrap_mode(texture_wrap_dimension dim, texture_wrap_mode mode) {
            glTexParameteri(GL_TEXTURE_CUBE_MAP, static_cast<GLenum>(dim),
                            static_cast<GLenum>(mode));
        }

        void set_border_color(glm::vec4 color) {
            glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color));
        }
    };
}
#endif //RENDEN_TEXTURE_HPP
