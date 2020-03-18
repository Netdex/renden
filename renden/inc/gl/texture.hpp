#ifndef RENDEN_TEXTURE_HPP
#define RENDEN_TEXTURE_HPP

#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>
#include <nonstd/span.hpp>

namespace gl
{
enum TextureWrapMode : GLenum
{
	REPEAT = GL_REPEAT,
	MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
	CLAMP_EDGE = GL_CLAMP_TO_EDGE,
	CLAMP_BORDER = GL_CLAMP_TO_BORDER
};

enum TextureWrapDimension : GLenum
{
	S = GL_TEXTURE_WRAP_S,
	T = GL_TEXTURE_WRAP_T,
	R = GL_TEXTURE_WRAP_R
};

enum TextureFilterMode : GLenum
{
	NEAREST = GL_NEAREST,
	LINEAR = GL_LINEAR,
	NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
	LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
	NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
	LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
};

enum TextureFilterCase : GLenum
{
	SCALE_UP = GL_TEXTURE_MAG_FILTER,
	SCALE_DOWN = GL_TEXTURE_MIN_FILTER
};

/**
 * \brief 1D texture from data buffer
 * \tparam T Data type of buffer
 * TODO Use texture buffers instead?
 */
template <GLenum internalformat, GLenum format, GLenum type, typename T = GLfloat>
class Texture1D
{
	GLuint id_;
	GLuint tex_id_;
public:
	Texture1D(nonstd::span<T> data, GLuint tex_id) : tex_id_(tex_id)
	{
		glGenTextures(1, &id_);
		this->Bind();
		glTexImage1D(GL_TEXTURE_1D, 0, internalformat, data.size(), 0, format, type, data.data());
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAX_LEVEL, 0);
	}

	~Texture1D()
	{
		glDeleteTextures(1, &id_);
	}

	Texture1D(const Texture1D& o) = delete;
	Texture1D& operator=(const Texture1D& o) = delete;

	void Bind() const
	{
		glActiveTexture(GL_TEXTURE0 + tex_id_);
		glBindTexture(GL_TEXTURE_1D, id_);
	}
};

/**
 * \brief 2D texture array from files
 */
class Texture2D
{
	GLuint id_;
	GLuint tex_id_;
public:

	Texture2D(const std::string paths[], size_t count,
	          unsigned int width, unsigned int height,
	          TextureFilterMode filter_mode, TextureWrapMode wrap_mode,
	          GLuint tex_id) : tex_id_(tex_id)
	{
		glGenTextures(1, &id_);
		this->Bind();

		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, static_cast<GLsizei>(count));

		for (unsigned i = 0; i < count; i++)
		{
			const std::string& path = paths[i];
			int w, h, bpp;
			unsigned char* imgbuf = stbi_load(path.c_str(), &w, &h, &bpp, 4);
			assert(imgbuf != nullptr && int(width) == w && int(height) == h);
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, w, h, 1,
			                GL_RGBA, GL_UNSIGNED_BYTE, imgbuf);
			stbi_image_free(imgbuf);
		}

		this->SetFilterMode(SCALE_UP, filter_mode);
		this->SetFilterMode(SCALE_DOWN, filter_mode);
		this->SetWrapMode(S, wrap_mode);
		this->SetWrapMode(T, wrap_mode);
		if (filter_mode == NEAREST_MIPMAP_NEAREST || filter_mode == LINEAR_MIPMAP_NEAREST
			|| filter_mode == NEAREST_MIPMAP_LINEAR || filter_mode == LINEAR_MIPMAP_LINEAR)
		{
			glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
		}
	}

	Texture2D(const std::vector<std::string>& paths, unsigned int width, unsigned int height,
	          TextureFilterMode filter_mode, TextureWrapMode wrap_mode,
	          unsigned int tex_id) : Texture2D(&paths[0], paths.size(), width, height,
	                                           filter_mode, wrap_mode, tex_id)
	{
	}

	~Texture2D()
	{
		glDeleteTextures(1, &id_);
	}

	Texture2D(const Texture2D& o) = delete;
	Texture2D operator=(const Texture2D& o) = delete;

	void Bind() const
	{
		glActiveTexture(GL_TEXTURE0 + tex_id_);
		glBindTexture(GL_TEXTURE_2D_ARRAY, id_);
	}

	void SetFilterMode(TextureFilterCase cse, TextureFilterMode mode)
	{
		glTexParameteri(GL_TEXTURE_2D_ARRAY, static_cast<GLenum>(cse), mode);
	}

	void SetWrapMode(TextureWrapDimension dim, TextureWrapMode mode)
	{
		glTexParameteri(GL_TEXTURE_2D_ARRAY, static_cast<GLenum>(dim), mode);
	}

	void SetBorderColor(glm::vec4 color)
	{
		glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, value_ptr(color));
	}
};

class Cubemap
{
	GLuint id_;
	GLuint tex_id_;
public:

	Cubemap(const std::string paths[6],
	        TextureFilterMode filter_mode, TextureWrapMode wrap_mode,
	        GLuint tex_id) : tex_id_(tex_id)
	{
		glGenTextures(1, &id_);
		this->Bind();

		for (int i = 0; i < 6; i++)
		{
			const std::string& path = paths[i];
			int w, h, bpp;
			unsigned char* imgbuf = stbi_load(path.c_str(), &w, &h, &bpp, 3);
			assert(imgbuf != nullptr);
			glTexImage2D(GLenum(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i), 0, GL_RGB, w, h, 0,
			             GL_RGB, GL_UNSIGNED_BYTE, imgbuf);
			stbi_image_free(imgbuf);
		}

		this->SetFilterMode(SCALE_UP, filter_mode);
		this->SetFilterMode(SCALE_DOWN, filter_mode);
		this->SetWrapMode(S, wrap_mode);
		this->SetWrapMode(T, wrap_mode);
		this->SetWrapMode(R, wrap_mode);

		if (filter_mode == NEAREST_MIPMAP_NEAREST || filter_mode == LINEAR_MIPMAP_NEAREST
			|| filter_mode == NEAREST_MIPMAP_LINEAR || filter_mode == LINEAR_MIPMAP_LINEAR)
		{
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		}
	}

	Cubemap(const std::vector<std::string>& paths,
	        TextureFilterMode filter_mode, TextureWrapMode wrap_mode,
	        unsigned int tex_id) : Cubemap((assert(paths.size() == 6), &paths[0]),
	                                       filter_mode, wrap_mode, tex_id)
	{
	}

	~Cubemap()
	{
		glDeleteTextures(1, &id_);
	}

	Cubemap(const Texture2D& o) = delete;
	Cubemap operator=(const Texture2D& o) = delete;

	void Bind()
	{
		glActiveTexture(GL_TEXTURE0 + tex_id_);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id_);
	}

	void SetFilterMode(TextureFilterCase cse, TextureFilterMode mode)
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP, static_cast<GLenum>(cse),
		                static_cast<GLenum>(mode));
	}

	void SetWrapMode(TextureWrapDimension dim, TextureWrapMode mode)
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP, static_cast<GLenum>(dim),
		                static_cast<GLenum>(mode));
	}

	void SetBorderColor(glm::vec4 color)
	{
		glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, value_ptr(color));
	}
};
}
#endif //RENDEN_TEXTURE_HPP
