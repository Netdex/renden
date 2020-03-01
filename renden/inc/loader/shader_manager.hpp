#ifndef RENDEN_SHADER_MANAGER_HPP
#define RENDEN_SHADER_MANAGER_HPP

#include <vector>

#include "gl/varray.hpp"
#include "gl/shader.hpp"
#include "world/skybox.hpp"

namespace shaders
{
namespace block
{
	extern std::weak_ptr<gl::Shader> shader;
	extern std::vector<gl::VArrayAttribute> attribs;

	std::shared_ptr<gl::Shader> Load();
}

namespace tenbox
{
	extern std::weak_ptr<gl::Shader> shader;
	extern std::vector<gl::VArrayAttribute> attribs;
	extern std::shared_ptr<Skybox> tenbox;

	std::shared_ptr<gl::Shader> Load();
}

namespace reticle
{
	extern std::weak_ptr<gl::Shader> shader;
	extern std::vector<gl::VArrayAttribute> attribs;

	std::shared_ptr<gl::Shader> Load();
}

// we keep a reference so we can control when it's destroyed
std::vector<std::shared_ptr<gl::Shader>> Load();
}


#endif //RENDEN_SHADER_MANAGER_HPP
