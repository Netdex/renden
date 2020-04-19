#ifndef RENDEN_GL_DEPTHMAP_HPP_
#define RENDEN_GL_DEPTHMAP_HPP_

#include <nonstd/span.hpp>

#include "gl/texture.hpp"

namespace gl
{
class DepthMap : public Texture2D<GLfloat>
{
public:
	DepthMap(int width, int height, GLuint tex_id) : Texture2D(nonstd::span<GLfloat>{}, width, height, LINEAR, LINEAR,
	                                                           CLAMP_BORDER, tex_id, GL_DEPTH_COMPONENT,
	                                                           GL_DEPTH_COMPONENT, GL_FLOAT)
	{
		Bind();
		float borderColor[] = { 1.0f,1.0f,1.0f,1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		Unbind();
	}
};
}
#endif
