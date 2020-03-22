#include "gl/debug.hpp"

#include <sstream>

#include <spdlog/spdlog.h>

void APIENTRY debug_callback(GLenum source, GLenum type, GLuint id,
                             GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	std::stringstream output;
	output << "OGL:";

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		output << "Error";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		output << "Deprecated Behaviour";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		output << "Undefined Behaviour";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		output << "Portability";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		output << "Performance";
		break;
	case GL_DEBUG_TYPE_MARKER:
		output << "Marker";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		output << "Push Group";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		output << "Pop Group";
		break;
	case GL_DEBUG_TYPE_OTHER:
		output << "Other";
		break;
	default: break;
	}
	output << ":";
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:
		output << "API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		output << "Window System";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		output << "Shader Compiler";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		output << "Third Party";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		output << "Application";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		output << "Other";
		break;
	default: break;
	}
	output << " - " << message;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		spdlog::error(output.str());
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		spdlog::warn(output.str());
		break;
	case GL_DEBUG_SEVERITY_LOW:
		spdlog::info(output.str());
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		spdlog::trace(output.str());
		break;
	default: break;
	}
}
