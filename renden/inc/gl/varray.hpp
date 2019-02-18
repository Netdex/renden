//
// Created by netdex on 2/13/19.
//

#ifndef RENDEN_VARRAY_HPP
#define RENDEN_VARRAY_HPP

#include <glad/glad.h>

namespace gl {
    enum varray_attribute_type : GLenum {
        BYTE = GL_BYTE,
        UBYTE = GL_UNSIGNED_BYTE,
        SHORT = GL_SHORT,
        USHORT = GL_UNSIGNED_SHORT,
        INT = GL_INT,
        UINT = GL_UNSIGNED_INT,
        HFLOAT = GL_HALF_FLOAT,
        FLOAT = GL_FLOAT,
        DOUBLE = GL_DOUBLE
    };

    struct varray_attribute {
        GLuint location;
        size_t size;
        varray_attribute_type type;
        unsigned int stride;
        unsigned int offset;
        bool normalized;
    };

    class varray {
        GLuint id;
    public:
        varray() {
            glGenVertexArrays(1, &id);
            this->bind();
        }

        varray(const varray &o) = delete;
        varray operator=(const varray &o) = delete;

        void bind() {
            glBindVertexArray(id);
        }

        void unbind(){
            glBindVertexArray(0);
        }

        void set_attrib(GLuint location, size_t size, varray_attribute_type type,
                        unsigned int stride, unsigned int offset, bool normalized = false) {
            glVertexAttribPointer(location, static_cast<GLint>(size),
                                  static_cast<GLenum>(type), static_cast<GLboolean>(normalized),
                                  static_cast<GLsizei>(stride), reinterpret_cast<const GLvoid *>(offset));
            glEnableVertexAttribArray(location);

        }
    };
}
#endif //RENDEN_VARRAY_HPP
