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
	DIM_S = GL_TEXTURE_WRAP_S,
	DIM_T = GL_TEXTURE_WRAP_T,
	DIM_R = GL_TEXTURE_WRAP_R
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

enum TextureTarget : GLenum
{
	TEXTURE_1D = GL_TEXTURE_1D,
	TEXTURE_2D = GL_TEXTURE_2D,
	TEXTURE_2D_ARRAY = GL_TEXTURE_2D_ARRAY,
	TEXTURE_CUBEMAP = GL_TEXTURE_CUBE_MAP
};

class Texture
{
public:
	virtual ~Texture()
	{
		glDeleteTextures(1, &id_);
	}

	Texture(const Texture& o) = delete;
	Texture& operator=(const Texture& o) = delete;

	void Bind() const
	{
		glActiveTexture(GL_TEXTURE0 + tex_id_);
		glBindTexture(target_, id_);
	}

	void Unbind() const
	{
#ifndef NDEBUG
		glActiveTexture(GL_TEXTURE0 + tex_id_);
		glBindTexture(target_, 0);
#endif
	}

	TextureTarget GetTarget() const
	{
		return target_;
	}

	GLuint GetID() const
	{
		return id_;
	}

protected:
	TextureTarget target_;
	GLuint id_{};
	GLuint tex_id_{};

	Texture(TextureTarget target, GLuint tex_id) : target_(target), tex_id_(tex_id)
	{
	}


	void SetFilterMode(TextureFilterCase cse, TextureFilterMode mode) const
	{
		glTexParameteri(target_, static_cast<GLenum>(cse), mode);
	}

	void SetWrapMode(TextureWrapDimension dim, TextureWrapMode mode) const
	{
		glTexParameteri(target_, static_cast<GLenum>(dim), mode);
	}

	void SetBorderColor(glm::vec4 color) const
	{
		glTexParameterfv(target_, GL_TEXTURE_BORDER_COLOR, value_ptr(color));
	}
};

/**
 * \brief 1D texture from data buffer
 * \tparam U Data type of buffer
 */
template <typename U = GLfloat>
class Texture1D : public Texture
{
public:
	Texture1D(nonstd::span<U> data, GLuint tex_id, GLenum internalformat, GLenum format, GLenum type) : Texture(
		TEXTURE_1D, tex_id)
	{
		glGenTextures(1, &id_);
		Bind();
		// Width is in texels!
		glTexImage1D(GL_TEXTURE_1D, 0, internalformat, data.size() / 4, 0, format, type, data.data());
		SetFilterMode(SCALE_UP, NEAREST);
		SetFilterMode(SCALE_DOWN, NEAREST);
		SetWrapMode(DIM_S, CLAMP_EDGE);
		SetWrapMode(DIM_T, CLAMP_EDGE);
		Unbind();
	}
};


template <typename U = GLfloat>
class Texture2D : public Texture
{
public:
	Texture2D(nonstd::span<U> data, int width, int height, TextureFilterMode min_filter_mode,
	          TextureFilterMode mag_filter_mode,
	          TextureWrapMode wrap_mode, GLuint tex_id, GLenum internalformat, GLenum format,
	          GLenum type) : Texture(TEXTURE_2D, tex_id)
	{
		glGenTextures(1, &id_);
		Bind();

		glTexImage2D(TEXTURE_2D, 0, internalformat, width, height, 0, format, type, data.data());

		SetFilterMode(SCALE_UP, mag_filter_mode);
		SetFilterMode(SCALE_DOWN, min_filter_mode);
		SetWrapMode(DIM_S, wrap_mode);
		SetWrapMode(DIM_T, wrap_mode);
		Unbind();
	}
};

class Texture2DArray : public Texture
{
public:
	/*
	 * \brief Create an empty texture 2D array
	 */
	Texture2DArray(int width, int height, int size, TextureFilterMode min_filter_mode,
	               TextureFilterMode mag_filter_mode, TextureWrapMode wrap_mode, int mipmap_levels, GLuint tex_id,
	               GLenum internalformat)
		: Texture(TEXTURE_2D_ARRAY, tex_id),
		  width_(width), height_(height), size_(size)
	{
		glGenTextures(1, &id_);
		Bind();

		assert(mipmap_levels >= 1);
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipmap_levels, internalformat, width, height, GLsizei(size));
		SetFilterMode(SCALE_UP, mag_filter_mode);
		SetFilterMode(SCALE_DOWN, min_filter_mode);
		SetWrapMode(DIM_S, wrap_mode);
		SetWrapMode(DIM_T, wrap_mode);

		Unbind();
	}

	/*
	 * \brief Create a texture 2D array from image files
	 */
	Texture2DArray(nonstd::span<std::string> paths,
	               int width, int height,
	               TextureFilterMode min_filter_mode, TextureFilterMode mag_filter_mode,
	               TextureWrapMode wrap_mode,
	               int mipmap_levels,
	               GLuint tex_id) : Texture(TEXTURE_2D_ARRAY, tex_id),
	                                width_(width), height_(height), size_(paths.size())
	{
		glGenTextures(1, &id_);
		Bind();

		assert(mipmap_levels >= 1);
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipmap_levels, GL_RGBA8, width, height, static_cast<GLsizei>(paths.size()));

		for (unsigned i = 0; i < paths.size(); i++)
		{
			const std::string& path = paths[i];
			int w, h, bpp;
			unsigned char* imgbuf = stbi_load(path.c_str(), &w, &h, &bpp, 4);
			assert(imgbuf != nullptr && int(width) == w && int(height) == h);
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, w, h, 1,
			                GL_RGBA, GL_UNSIGNED_BYTE, imgbuf);
			stbi_image_free(imgbuf);
		}

		SetFilterMode(SCALE_UP, mag_filter_mode);
		SetFilterMode(SCALE_DOWN, min_filter_mode);
		SetWrapMode(DIM_S, wrap_mode);
		SetWrapMode(DIM_T, wrap_mode);

		if (min_filter_mode == NEAREST_MIPMAP_NEAREST || min_filter_mode == LINEAR_MIPMAP_NEAREST
			|| min_filter_mode == NEAREST_MIPMAP_LINEAR || min_filter_mode == LINEAR_MIPMAP_LINEAR)
		{
			glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
		}
		Unbind();
	}

	int GetWidth() const { return width_; }
	int GetHeight() const { return height_; }
	int GetSize() const { return size_; }
private:
	int width_, height_, size_;
};

class Cubemap : public Texture
{
public:

	Cubemap(const std::string paths[6],
	        TextureFilterMode filter_mode, TextureWrapMode wrap_mode,
	        GLuint tex_id) : Texture(TEXTURE_CUBEMAP, tex_id)
	{
		glGenTextures(1, &id_);
		Bind();

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

		SetFilterMode(SCALE_UP, filter_mode);
		SetFilterMode(SCALE_DOWN, filter_mode);
		SetWrapMode(DIM_S, wrap_mode);
		SetWrapMode(DIM_T, wrap_mode);
		SetWrapMode(DIM_R, wrap_mode);

		if (filter_mode == NEAREST_MIPMAP_NEAREST || filter_mode == LINEAR_MIPMAP_NEAREST
			|| filter_mode == NEAREST_MIPMAP_LINEAR || filter_mode == LINEAR_MIPMAP_LINEAR)
		{
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		}
		Unbind();
	}
};
}
#endif //RENDEN_TEXTURE_HPP
