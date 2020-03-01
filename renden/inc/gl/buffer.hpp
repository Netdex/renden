#ifndef RENDEN_BUFFER_HPP
#define RENDEN_BUFFER_HPP

#include <vector>

#include "glad/glad.h"

namespace gl
{
enum BufferType : GLenum
{
	VERTEX_BUFFER = GL_ARRAY_BUFFER,
	ELEMENT_BUFFER = GL_ELEMENT_ARRAY_BUFFER
	// Add as necessary.
};

enum BufferUsage : GLenum
{
	STATIC_DRAW = GL_STATIC_DRAW,
	DYNAMIC_DRAW = GL_DYNAMIC_DRAW
	// Add as necessary.
};


/**
 * \brief C++ wrapper around a general OpenGL buffer (either a vertex buffer or element buffer).
 * \tparam T OpenGL type of the element the buffer is holding.
 */
template <typename T>
class Buffer
{
	GLuint id_{};
	size_t cached_size_ = 0;

	BufferType type_;
	BufferUsage usage;
public:

	Buffer(BufferType type, BufferUsage usage)
		: type_(type), usage(usage)
	{
		glGenBuffers(1, &this->id_);
		this->Bind();
	}

	Buffer(BufferType type, BufferUsage usage, const T* data, size_t size)
		: type_(type), usage(usage)
	{
		glGenBuffers(1, &this->id_);
		this->Allocate(data, size);
		this->Bind();
	}

	explicit Buffer(BufferType type, BufferUsage usage, const std::vector<T>& data) : Buffer(
		type, usage, &data[0], data.size() * sizeof(data[0]))
	{
	}

	Buffer(const T* data, size_t size)
		: Buffer(VERTEX_BUFFER, STATIC_DRAW, data, size)
	{
	}

	explicit Buffer(const std::vector<T>& data)
		: Buffer(VERTEX_BUFFER, STATIC_DRAW, data)
	{
	}

	~Buffer()
	{
		glDeleteBuffers(1, &id_);
	}

	Buffer(const Buffer& o) = delete;
	Buffer operator=(const Buffer& o) = delete;

	void Bind()
	{
		glBindBuffer(static_cast<GLenum>(type_), this->id_);
	}

	void Unbind() const
	{
		glBindBuffer(static_cast<GLenum>(type_), 0);
	}

	size_t GetSize()
	{
		if (cached_size_ == 0)
		{
			this->Bind();
			GLint size;
			glGetBufferParameteriv(type_, GL_BUFFER_SIZE, &size);
			cached_size_ = size;
			this->Unbind();
			return static_cast<size_t>(size);
		}
		return cached_size_;
	}

	size_t GetElementCount()
	{
		return this->GetSize() / sizeof(T);
	}

	//T *map(/* access */);

	void Allocate(const T* data, size_t size)
	{
		cached_size_ = 0;
		this->Bind();
		glBufferData(static_cast<GLenum>(type_),
		             static_cast<GLsizeiptr>(size), static_cast<const GLvoid*>(data),
		             static_cast<GLenum>(usage));
		this->Unbind();
	}

	void Update(unsigned int offset, const T* data, size_t size)
	{
		this->Bind();
		glBufferSubData(type_, static_cast<GLintptr>(offset),
		                static_cast<GLsizeiptr>(size), static_cast<const GLvoid*>(data));
		this->Unbind();
	}

	void Update(unsigned int offset, const std::vector<T>& data)
	{
		this->Update(offset, &data.begin(), data.size());
	}
};
}

#endif //RENDEN_BUFFER_HPP
