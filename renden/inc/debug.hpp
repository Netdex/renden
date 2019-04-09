
#ifndef RENDEN_DEBUG_HPP
#define RENDEN_DEBUG_HPP

#include "glitter.hpp"

void APIENTRY debug_callback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
	printf("[");
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		printf("High");
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		printf("Medium");
		break;
	case GL_DEBUG_SEVERITY_LOW:
		printf("Low");
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		printf("Notification");
		break;
	}
	printf("] ");

	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		printf("Error");
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		printf("Deprecated Behaviour");
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		printf("Undefined Behaviour");
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		printf("Portability");
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		printf("Performance");
		break;
	case GL_DEBUG_TYPE_MARKER:
		printf("Marker");
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		printf("Push Group");
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		printf("Pop Group");
		break;
	case GL_DEBUG_TYPE_OTHER:
		printf("Other");
		break;
	}
	printf(" - ");
	switch (source) {
	case GL_DEBUG_SOURCE_API:
		printf("API");
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		printf("Window System");
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		printf("Shader Compiler");
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		printf("Third Party");
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		printf("Application");
		break;
	case GL_DEBUG_SOURCE_OTHER:
		printf("Other");
		break;
	}
	printf(": %s\n", message);
}
#endif