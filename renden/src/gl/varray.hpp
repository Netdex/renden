//
// Created by netdex on 2/13/19.
//

#ifndef RENDEN_VARRAY_HPP
#define RENDEN_VARRAY_HPP

#include <cassert>

#include <glad/glad.h>

namespace gl
{
enum VArrayAttributeType : GLenum
{
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

struct VArrayAttribute
{
	GLuint location;
	size_t size;
	VArrayAttributeType type;
	unsigned int stride;
	unsigned int offset;
	bool normalized;

	GLuint divisor;
};

class VArray
{
	GLuint id_{};
public:
	VArray()
	{
		glGenVertexArrays(1, &id_);
		this->Bind();
	}

	~VArray()
	{
		glDeleteVertexArrays(1, &id_);
	}

	VArray(const VArray& o) = delete;
	VArray operator=(const VArray& o) = delete;

	void Bind() const
	{
		glBindVertexArray(id_);
	}

	void Unbind() const
	{
		glBindVertexArray(0);
	}

	void SetAttribute(GLuint location, size_t size, VArrayAttributeType type,
	                  unsigned int stride, unsigned int offset, bool normalized = false, GLuint divisor = 0) const
	{
		switch (type)
		{
		case INT:
		case UINT:
		case BYTE:
		case UBYTE:
		case SHORT:
		case USHORT:
			glVertexAttribIPointer(location, GLint(size), GLenum(type), GLsizei(stride),
			                       reinterpret_cast<const GLvoid*>(intptr_t(offset)));
			break;
		case HFLOAT:
		case FLOAT:
			glVertexAttribPointer(location, GLint(size), GLenum(type), GLboolean(normalized), GLsizei(stride),
			                      reinterpret_cast<const GLvoid *>(intptr_t(offset)));
			break;
		case DOUBLE:
			glVertexAttribLPointer(location, GLint(size), GLenum(type), GLsizei(stride),
			                       reinterpret_cast<const GLvoid *>(intptr_t(offset)));
			break;
		default:
			assert(false);
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
