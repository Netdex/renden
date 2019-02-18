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
                STATIC_DRAW = GL_STATIC_DRAW
    };

    template<buffer_type type, buffer_usage usage, typename T>
    class buffer {
        GLuint id;
        size_t cached_size = 0;
    public:

        buffer(const T *data, size_t size) {
            glGenBuffers(1, &this->id);
            this->bind();
            glBufferData(static_cast<GLenum>(type),
                         static_cast<GLsizeiptr>(size), static_cast<const GLvoid *>(data),
                         static_cast<GLenum>(usage));
        }

        explicit buffer(const std::vector<T> &data)
                : buffer(&data[0], data.size() * sizeof(data[0])) {}

        ~buffer() {
            glDeleteBuffers(1, &this->id);
        }

        buffer(const buffer &o) = delete;

        buffer operator=(const buffer &o) = delete;

        void bind() {
            glBindBuffer(static_cast<GLenum>(type), this->id);
        }

        size_t get_size() {
            if (cached_size == 0) {
                GLint size;
                glGetBufferParameteriv(type, GL_BUFFER_SIZE, &size);
                cached_size = size;
                return static_cast<size_t>(size);
            } else {
                return cached_size;
            }
        }

        size_t get_elements() {
            return this->get_size() / sizeof(T);
        }

        //T *map(/* access */);

        void update(unsigned int offset, const T *data, size_t size) {
            glBufferSubData(type, static_cast<GLintptr>(offset),
                            static_cast<GLsizeiptr>(size), static_cast<const GLvoid *>(data));
        }

        void update(unsigned int offset, const std::vector<T> &data) {
            this->update(offset, &data.begin(), data.size());
        }
    };
}


#endif //RENDEN_BUFFER_HPP
