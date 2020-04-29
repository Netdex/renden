#ifndef RENDEN_GL_FRAMEBUFFER_HPP_
#define RENDEN_GL_FRAMEBUFFER_HPP_

#include <glad/glad.h>

#include "texture.hpp"

namespace gl
{
enum class ColorBuffer : GLenum
{
	NONE = GL_NONE
};

class FrameBuffer
{
public:

	FrameBuffer(ColorBuffer read_buffer, ColorBuffer draw_buffer)
	{
		glGenFramebuffers(1, &fbo_);
		Bind();
		glReadBuffer(static_cast<GLenum>(read_buffer));
		glDrawBuffer(static_cast<GLenum>(draw_buffer));
		Unbind();
	}

	void Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
	}

	void Unbind() const
	{
		assert(this->IsActiveFrameBuffer());
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Attach(const Texture& texture, GLenum attachment) const
	{
		assert(this->IsActiveFrameBuffer());
		switch (texture.GetTarget())
		{
		case TEXTURE_2D:
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texture.GetTarget(), texture.GetID(), 0);
			break;
		default:
			assert(false);
		}
	}

	void Attach(const Texture& texture, GLenum attachment, GLint layer) const
	{
		assert(this->IsActiveFrameBuffer());
		switch (texture.GetTarget())
		{
		case TEXTURE_2D_ARRAY:
			glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, texture.GetID(), 0, layer);
			break;
		default:
			assert(false);
		}
	}
private:

#ifndef NDEBUG
	bool IsActiveFrameBuffer() const
	{
		GLint id;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &id);
		return id == GLint(fbo_);
	}
#endif

	GLuint fbo_{};
};
}
#endif
