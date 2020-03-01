//
// Created by netdex on 2019-02-18.
//

#ifndef RENDEN_SKYBOX_HPP
#define RENDEN_SKYBOX_HPP


#include <gl/texture.hpp>
#include <gl/mesh.hpp>

class Skybox
{
	std::shared_ptr<gl::Cubemap> texture_;
	std::shared_ptr<gl::Mesh<>> mesh_;
public:
	Skybox(const std::vector<std::string>& paths);

	void Draw(const gl::Shader& shader) const;
};


#endif //RENDEN_SKYBOX_HPP
