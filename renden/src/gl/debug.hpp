#ifndef RENDEN_DEBUG_HPP
#define RENDEN_DEBUG_HPP

#include "glitter.hpp"

void APIENTRY debug_callback(GLenum source, GLenum type, GLuint id,
                             GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

#endif
