#ifndef RENDEN_GL_FRAMEBUFFER_HPP_
#define RENDEN_GL_FRAMEBUFFER_HPP_

#include <glad/glad.h>

#include "texture.hpp"

namespace gl
{
class FrameBuffer
{
	GLuint fbo_{};

public:

	FrameBuffer()
	{
		glGenFramebuffers(1, &fbo_);
	}

	void Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
	}

	static void Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	static void Attach(const Texture& texture, GLenum attachment)
	{
		switch(texture.GetTarget())
		{
		case TEXTURE_2D:
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texture.GetTarget(), texture.GetID(), 0);
			break;
		default:
			assert(false);
		}
	}

	static void Attach(const Texture& texture, GLenum attachment, GLint layer)
	{
		switch(texture.GetTarget())
		{
		case TEXTURE_2D_ARRAY:
			glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, texture.GetID(), 0, layer);
			break;
		default:
			assert(false);
		}
	}
};
}
#endif
