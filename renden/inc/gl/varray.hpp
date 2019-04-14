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

        GLuint divisor;
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

        ~varray() {
            glDeleteVertexArrays(1, &id);
        }

        void bind() {
            glBindVertexArray(id);
        }

        void unbind() {
            glBindVertexArray(0);
        }

        void set_attrib(GLuint location, size_t size, varray_attribute_type type,
                        unsigned int stride, unsigned int offset, bool normalized = false, GLuint divisor = 0) {
            switch(type){
                case gl::INT:
                case gl::UINT:
                case gl::BYTE:
                case gl::UBYTE:
                case gl::SHORT:
                case gl::USHORT:
                    glVertexAttribIPointer(location, static_cast<GLint>(size),
                                          static_cast<GLenum>(type),
                                          static_cast<GLsizei>(stride), reinterpret_cast<const GLvoid *>(offset));
                    break;
                case gl::HFLOAT:
                case gl::FLOAT:
                    glVertexAttribPointer(location, static_cast<GLint>(size),
                                          static_cast<GLenum>(type), static_cast<GLboolean>(normalized),
                                          static_cast<GLsizei>(stride), reinterpret_cast<const GLvoid *>(offset));
                    break;
                case gl::DOUBLE:
                    glVertexAttribLPointer(location, static_cast<GLint>(size),
                                           static_cast<GLenum>(type),
                                           static_cast<GLsizei>(stride), reinterpret_cast<const GLvoid *>(offset));
                    break;
            }

            glEnableVertexAttribArray(location);
            glVertexAttribDivisor(location, divisor);
        }
		/*void set_attribs(const std::vector<varray_attribute> &attribs) {
			for (auto i : attribs) {
				set_attrib(i.location, i.size, i.type, i.stride, i.offset, i.normalized, i.divisor);
			}
		}*/
      
    };
}
#endif //RENDEN_VARRAY_HPP
