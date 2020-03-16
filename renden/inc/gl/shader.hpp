#pragma once

#include <memory>
#include <string>
#include <fstream>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <spdlog/spdlog.h>
#include <nonstd/span.hpp>

namespace gl
{
class Shader
{
	GLuint program_;
	GLint status_;
	GLint length_;

public:
	Shader() { program_ = glCreateProgram(); }

	~Shader()
	{
		glDeleteProgram(program_);
	}

	Shader(Shader const&) = delete;

	Shader& operator=(Shader const&) = delete;

	// Public Member Functions
	Shader& Activate()
	{
		glUseProgram(program_);
		return *this;
	}

	Shader& Attach(std::string const& filename)
	{
		// Load GLSL Shader Source from File
		std::string path = PROJECT_SOURCE_DIR "/renden/shaders/";
		//    std::string path = "../../renden/shaders/";
		std::ifstream fd(path + filename);
		auto src = std::string(std::istreambuf_iterator<char>(fd),
		                             (std::istreambuf_iterator<char>()));

		// Create a Shader Object
		const char* source = src.c_str();
		auto shader = Create(filename);
		glShaderSource(shader, 1, &source, nullptr);
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status_);

		// Display the Build Log on Error
		if (!static_cast<bool>(status_))
		{
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length_);
			std::unique_ptr<char[]> buffer(new char[length_]);
			glGetShaderInfoLog(shader, length_, nullptr, buffer.get());
			spdlog::error("OGL:Shader - {}\n{}", filename.c_str(), buffer.get());
		}

		// Attach the Shader and Free Allocated Memory
		glAttachShader(program_, shader);
		glDeleteShader(shader);
		return *this;
	}

	static GLuint Create(std::string const& filename)
	{
		const auto index = filename.rfind(".");
		const auto ext = filename.substr(index + 1);
		if (ext == "comp") return glCreateShader(GL_COMPUTE_SHADER);
		if (ext == "frag") return glCreateShader(GL_FRAGMENT_SHADER);
		if (ext == "geom") return glCreateShader(GL_GEOMETRY_SHADER);
		if (ext == "vert") return glCreateShader(GL_VERTEX_SHADER);
		return false;
	}


	GLuint get() const { return program_; }

	Shader& Link()
	{
		glLinkProgram(program_);
		glGetProgramiv(program_, GL_LINK_STATUS, &status_);
		if (status_ == false)
		{
			glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &length_);
			std::unique_ptr<char[]> buffer(new char[length_]);
			glGetProgramInfoLog(program_, length_, nullptr, buffer.get());
			spdlog::error("OGL:Shader - {}", buffer.get());
		}
		assert(status_ == true);
		return *this;
	}

	GLint GetUniform(std::string const& name) const
	{
		return glGetUniformLocation(program_, name.c_str());
	}

	GLuint GetAttribute(std::string const& name) const
	{
		GLint attribute_location = glGetAttribLocation(program_, name.c_str());
		assert(attribute_location >= 0);
		return static_cast<GLuint>(attribute_location);
	}

	// Wrap Calls to glUniform
	void Bind(GLint location, float value) const
	{
		glUniform1f(location, value);
	}

	void Bind(GLint location, glm::vec2 value) const
	{
		glUniform2f(location, value.x, value.y);
	}

	void Bind(GLint location, glm::vec3 value) const
	{
		glUniform3f(location, value.x, value.y, value.z);
	}

	void Bind(GLint location, glm::vec4 value) const
	{
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void Bind(GLint location, glm::mat4 const& matrix) const
	{
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(matrix));
	}

	void Bind(GLint location, nonstd::span<float> data) const
	{
		glUniform1fv(location, static_cast<GLsizei>(data.size()), data.data());
	}

	template <typename T>
	void Bind(std::string const& name, T&& value, size_t size) const
	{
		GLint location = glGetUniformLocation(program_, name.c_str());
		if (location == -1) spdlog::error("OGL:Shader - Missing Uniform: {}", name.c_str());
		else
		{
			Bind(location, std::forward<T>(value), size);
		}
	}

	template <typename T>
	void Bind(std::string const& name, T&& value) const
	{
		GLint location = glGetUniformLocation(program_, name.c_str());
		if (location == -1) spdlog::error("OGL:Shader - Missing Uniform: {}", name.c_str());
		else
		{
			Bind(location, std::forward<T>(value));
		}
	}
};
}
