//
// Created by netdex on 2/13/19.
//

#ifndef RENDEN_BUFFER_HPP
#define RENDEN_BUFFER_HPP


#include <glad/glad.h>
#include <vector>

namespace gl {
    enum buffer_type : GLenum {
        /* add as necessary */
                VERTEX_BUFFER = GL_ARRAY_BUFFER,
        ELEMENT_BUFFER = GL_ELEMENT_ARRAY_BUFFER
    };
    enum buffer_usage : GLenum {
        /* add as necessary */
                STATIC_DRAW = GL_STATIC_DRAW,
                DYNAMIC_DRAW = GL_DYNAMIC_DRAW
    };

    template<typename T>
    class buffer {
        GLuint id;
        size_t cached_size = 0;

        buffer_type type;
        buffer_usage usage;
    public:

        buffer(buffer_type type, buffer_usage usage)
                : type(type), usage(usage) {
            glGenBuffers(1, &this->id);
            this->bind();
        }

        buffer(buffer_type type, buffer_usage usage, const T *data, size_t size)
                : type(type), usage(usage) {
            glGenBuffers(1, &this->id);
            this->allocate(data, size);
            this->bind();
        }

        explicit buffer(buffer_type type, buffer_usage usage, const std::vector<T> &data)
                : buffer(type, usage, &data[0], data.size() * sizeof(data[0])) {}

        buffer(const T *data, size_t size)
                : buffer(VERTEX_BUFFER, STATIC_DRAW, data, size) {}

        buffer(const std::vector<T> &data)
                : buffer(VERTEX_BUFFER, STATIC_DRAW, data) {}

        ~buffer() {
            glDeleteBuffers(1, &id);
        }

        buffer(const buffer &o) = delete;

        buffer operator=(const buffer &o) = delete;

        void bind() {
            glBindBuffer(static_cast<GLenum>(type), this->id);
        }

        void unbind() {
            glBindBuffer(static_cast<GLenum>(type), 0);
        }

        size_t get_size() {
            if (cached_size == 0) {
                this->bind();
                GLint size;
                glGetBufferParameteriv(type, GL_BUFFER_SIZE, &size);
                cached_size = size;
                this->unbind();
                return static_cast<size_t>(size);
            } else {
                return cached_size;
            }
        }

        size_t get_elements() {
            return this->get_size() / sizeof(T);
        }

        //T *map(/* access */);

        void allocate(const T *data, size_t size) {
            this->bind();
            glBufferData(static_cast<GLenum>(type),
                         static_cast<GLsizeiptr>(size), static_cast<const GLvoid *>(data),
                         static_cast<GLenum>(usage));
            this->unbind();
        }

        void update(unsigned int offset, const T *data, size_t size) {
            this->bind();
            glBufferSubData(type, static_cast<GLintptr>(offset),
                            static_cast<GLsizeiptr>(size), static_cast<const GLvoid *>(data));
            this->unbind();
        }

        void update(unsigned int offset, const std::vector<T> &data) {
            this->update(offset, &data.begin(), data.size());
        }
    };
}


#endif //RENDEN_BUFFER_HPP
