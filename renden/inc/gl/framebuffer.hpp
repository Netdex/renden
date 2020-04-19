#ifndef RENDEN_GL_FRAMEBUFFER_HPP_
#define RENDEN_GL_FRAMEBUFFER_HPP_

#include <GL/gl.h>

#include "glad/glad.h"
#include "texture.hpp"

namespace gl
{
class Framebuffer
{
	GLuint fbo_;

public:

	Framebuffer()
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
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texture.GetTarget(), texture.GetID(), 0);
	}
};
}
#endif
