#ifndef RENDEN_MESH_HPP
#define RENDEN_MESH_HPP

#include <memory>

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <nonstd/span.hpp>

#include "buffer.hpp"
#include "varray.hpp"
#include "shader.hpp"

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

template <typename T = GLfloat>
class Mesh
{
	std::unique_ptr<VArray> vao_;
	std::vector<std::unique_ptr<Buffer<T>>> vbo_;
	std::unique_ptr<Buffer<GLuint>> ebo_;

	// TODO: Deprecate this member.
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


	///**
	// * \brief Constructs a mesh with multiple VBOs
	// * \param vertices List of list of vertices
	// * \param draw_mode Draw mode for the entire mesh
	// * \param attribs List of list vertex attributes, where vertex attribute list i corresponds to vertex list i
	// */
	//Mesh(const std::vector<std::vector<T>>& vertices, MeshDrawMode draw_mode,
	//     const std::vector<std::vector<VArrayAttribute>>& attribs)
	//	: vao_(std::make_shared<VArray>()), attrib_0_size_(attribs[0][0].stride),
	//	  DrawMode(draw_mode)
	//{
	//	assert(attribs.size() == vertices.size());
	//	for (int i = 0; i < vertices.size(); i++)
	//	{
	//		// Create and bind a VBO to the VAO, with the specified vertices
	//		vbo_.push_back(std::make_shared<Buffer<T>>(vertices[i]));
	//		// Set all vertex attributes corresponding to this VBO
	//		for (auto a : attribs[i])
	//		{
	//			vao_->SetAttribute(a.location, a.size, a.type, a.stride, a.offset, a.normalized);
	//			glVertexAttribDivisor(a.location, a.divisor);
	//		}
	//	}
	//	this->Unbind();
	//}

	///**
	// * \brief Constructs a mesh with a single VBO and EBO
	// * \param vertices List of vertices in VBO
	// * \param vert_size Vertex count
	// * \param elements EBO corresponding to single VBO
	// * \param elem_size Element count
	// * \param draw_mode Draw mode for the entire mesh
	// * \param attributes List of vertex attributes
	// * \param attrib_count Vertex attribute count
	// */
	//Mesh(const T* vertices, size_t vert_size, const GLuint* elements, size_t elem_size,
	//     MeshDrawMode draw_mode,
	//     const VArrayAttribute* attributes, size_t attrib_count)
	//// Note: The initialization order is actually very important, since the VAO is bound on construction,
	//// and the VBO/EBO are bound on construction as well.
	//	: vao_(std::make_shared<VArray>()),
	//	  vbo_({std::make_shared<Buffer<T>>(vertices, vert_size)}),
	//	  ebo_(std::make_shared<Buffer<GLuint>>(elements,
	//	                                        elem_size)),
	//	  attrib_0_size_(attributes[0].stride),
	//	  DrawMode(draw_mode)
	//{
	//	for (unsigned i = 0; i < attrib_count; i++)
	//	{
	//		const VArrayAttribute& a = attributes[i];
	//		vao_->SetAttribute(a.location, a.size, a.type, a.stride, a.offset, a.normalized);
	//		glVertexAttribDivisor(a.location, a.divisor);
	//	}

	//	this->Unbind();
	//}

	///**
	// * \brief Constructs a mesh with a single VBO
	// * \param vertices List of vertices in VBO
	// * \param vert_size Vertex count
	// * \param draw_mode Draw mode for the entire mesh
	// * \param attributes List of vertex attributes
	// * \param attrib_count Vertex attribute count
	// */
	//Mesh(const T* vertices, size_t vert_size,
	//     MeshDrawMode draw_mode,
	//     const VArrayAttribute* attributes, size_t attrib_count)
	//	: vao_(std::make_shared<VArray>()),
	//	  vbo_({std::make_shared<Buffer<T>>(vertices, vert_size)}),
	//	  attrib_0_size_(attributes[0].stride),
	//	  DrawMode(draw_mode)
	//{
	//	for (unsigned int i = 0; i < attrib_count; i++)
	//	{
	//		const VArrayAttribute& a = attributes[i];
	//		vao_->SetAttribute(a.location, a.size, a.type, a.stride, a.offset, a.normalized);
	//		glVertexAttribDivisor(a.location, a.divisor);
	//	}

	//	this->Unbind();
	//}

	///**
	// * \brief Constructs a mesh with a VBO and corresponding EBO, from vectors
	// * \param vertices List of vertices in VBO
	// * \param elements List of elements in EBO
	// * \param draw_mode Draw mode for the entire mesh
	// * \param attributes List of vertex attributes
	// */
	//Mesh(const std::vector<T>& vertices, const std::vector<GLuint>& elements,
	//     MeshDrawMode draw_mode, const std::vector<VArrayAttribute>& attributes)
	//	: Mesh(&vertices[0], vertices.size() * sizeof(T), &elements[0], elements.size() * sizeof(GLuint),
	//	       draw_mode, &attributes[0], attributes.size())
	//{
	//}

	///**
	// * \brief Constructs a mesh with a VBO, from vectors
	// * \param vertices List of vertices in VBO
	// * \param draw_mode Draw mode for the entire mesh
	// * \param attributes List of vertex attributes
	// */
	//Mesh(const std::vector<T>& vertices,
	//     MeshDrawMode draw_mode, const std::vector<VArrayAttribute>& attributes)
	//	: Mesh(&vertices[0], vertices.size() * sizeof(T),
	//	       draw_mode, &attributes[0], attributes.size())
	//{
	//}

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
		               static_cast<GLsizei>(ebo_->GetSize()), GL_UNSIGNED_INT, 0);
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

		glDrawArrays(static_cast<GLenum>(DrawMode), 0, vertex_count);
		this->Unbind();
	}

	void DrawInstanced(const Shader& shader)
	{
		// TODO
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
