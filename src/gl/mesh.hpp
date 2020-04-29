#ifndef RENDEN_MESH_HPP
#define RENDEN_MESH_HPP

#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <nonstd/span.hpp>

#include "gl/buffer.hpp"
#include "gl/shader.hpp"
#include "gl/varray.hpp"

namespace gl
{
enum MeshDrawMode : GLenum
{
	TRIANGLES = GL_TRIANGLES,
	TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
	LINE_STRIP = GL_LINE_STRIP,
	QUADS = GL_QUADS,
	LINES = GL_LINES,
	POINTS = GL_POINTS
};

/*
 * TODO: This class needs some changes to properly support multiple VBOs.
 * There is currently no use-case in Renden that requires this.
 */
template <typename T = GLfloat>
class Mesh
{
	std::unique_ptr<VArray> vao_;
	std::vector<std::unique_ptr<Buffer<T>>> vbo_;
	std::unique_ptr<Buffer<GLuint>> ebo_;

	size_t attrib_0_size_{};
public:
	glm::mat4 Model = glm::mat4(1.f);
	MeshDrawMode DrawMode;

	Mesh(nonstd::span<const T> vertices, nonstd::span<VArrayAttribute> attribs, MeshDrawMode draw_mode)
		: vao_(std::make_unique<VArray>()),
		  attrib_0_size_(attribs[0].stride),
		  DrawMode(draw_mode)
	{
		vbo_.push_back(std::make_unique<Buffer<T>>(VERTEX_BUFFER, STATIC_DRAW, vertices));
		for (const VArrayAttribute& a : attribs)
		{
			vao_->SetAttribute(a.location, a.size, a.type, a.stride, a.offset, a.normalized);
			glVertexAttribDivisor(a.location, a.divisor);
		}
		this->Unbind();
	}

	Mesh(nonstd::span<const T> vertices, nonstd::span<GLuint> elements, nonstd::span<VArrayAttribute> attribs,
	     MeshDrawMode draw_mode)
		: vao_(std::make_unique<VArray>()),
		  ebo_(std::make_unique<Buffer<GLuint>>(ELEMENT_BUFFER, STATIC_DRAW, elements)),
		  attrib_0_size_(attribs[0].stride),
		  DrawMode(draw_mode)
	{
		vbo_.push_back(std::make_unique<Buffer<T>>(VERTEX_BUFFER, STATIC_DRAW, vertices));
		for (const VArrayAttribute& a : attribs)
		{
			vao_->SetAttribute(a.location, a.size, a.type, a.stride, a.offset, a.normalized);
			glVertexAttribDivisor(a.location, a.divisor);
		}
		this->Unbind();
	}

	/**
	 * \brief Constructs a mesh with an empty VBO
	 * \param draw_mode Draw mode for the entire mesh
	 * \param attributes List of vertex attributes
	 */
	Mesh(nonstd::span<VArrayAttribute> attributes, MeshDrawMode draw_mode)
		: Mesh(nonstd::span<T>{}, attributes, draw_mode)
	{
	}

	~Mesh() = default;

	Mesh(const Mesh& o) = delete;
	Mesh operator=(const Mesh& o) = delete;

	/**
	 * \brief Binds the VAO of this mesh (delegate)
	 */
	void Bind() const
	{
		vao_->Bind();
	}

	/**
	 * \brief Unbinds the VAO of this mesh (delegate)
	 */
	void Unbind() const
	{
		vao_->Unbind();
	}

	/**
	 * \brief Render this mesh using elements
	 * \pre Mesh must have an EBO and at least one VBO
	 * \param shader Shader program to draw with, and bind relevant uniforms
	 */
	void DrawElements(const Shader& shader)
	{
		assert(ebo_);
		this->Bind();
		shader.Bind("model", Model);
		glDrawElements(static_cast<GLenum>(DrawMode),
		               static_cast<GLsizei>(ebo_->GetSize()), GL_UNSIGNED_INT, nullptr);
		this->Unbind();
	}

	/**
	 * \brief Render this mesh
	 * \pre Mesh must have at least one VBO
	 * \param shader Shader program to draw with, and bind relevant uniforms
	 */
	void Draw(const Shader& shader)
	{
		assert(!vbo_.empty());
		this->Bind();
		shader.Bind("model", Model);

		// TODO This piece of code assumes that there is a single VBO.
		assert(vbo_.size() == 1);
		const size_t vertex_count = vbo_[0]->GetSizeBytes() / attrib_0_size_;

		glDrawArrays(static_cast<GLenum>(DrawMode), 0, GLsizei(vertex_count));
		this->Unbind();
	}

	/**
	 * \brief Update the first VBO with the specified vertex data
	 * \param data Vertex data
	 */
	void BufferVertexData(nonstd::span<T> data)
	{
		assert(!vbo_.empty());
		this->vbo_[0]->Allocate(data);
	}
};
}


#endif //RENDEN_MESH_HPP
