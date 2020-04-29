#ifndef RENDEN_BUFFER_HPP
#define RENDEN_BUFFER_HPP

#include <vector>

#include <glad/glad.h>
#include <nonstd/span.hpp>

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

	Buffer(BufferType type, BufferUsage usage, nonstd::span<const T> data)
		: type_(type), usage(usage)
	{
		glGenBuffers(1, &this->id_);
		this->Allocate(data);
		this->Bind();
	}

	~Buffer()
	{
		glDeleteBuffers(1, &id_);
	}

	Buffer(const Buffer& o) = delete;
	Buffer operator=(const Buffer& o) = delete;

	void Bind() const
	{
		glBindBuffer(static_cast<GLenum>(type_), this->id_);
	}

	void Unbind() const
	{
#ifndef NDEBUG
		glBindBuffer(static_cast<GLenum>(type_), 0);
#endif
	}

	size_t GetSizeBytes()
	{
		if (cached_size_ == 0)
		{
			this->Bind();
			GLint size;
			glGetBufferParameteriv(type_, GL_BUFFER_SIZE, &size);
			cached_size_ = size;
			this->Unbind();
		}
		return cached_size_;
	}

	size_t GetSize()
	{
		return this->GetSizeBytes() / sizeof(T);
	}

	//T *map(/* access */);

	void Allocate(nonstd::span<const T> data)
	{
		cached_size_ = 0;
		this->Bind();
		glBufferData(static_cast<GLenum>(type_),
		             static_cast<GLsizeiptr>(data.size_bytes()), static_cast<const GLvoid*>(data.data()),
		             static_cast<GLenum>(usage));
		this->Unbind();
	}

	void Update(unsigned int offset, nonstd::span<T> data)
	{
		this->Bind();
		glBufferSubData(type_, static_cast<GLintptr>(offset),
		                static_cast<GLsizeiptr>(data.size()), static_cast<const GLvoid*>(data.data()));
		this->Unbind();
	}
};
}

#endif //RENDEN_BUFFER_HPP
