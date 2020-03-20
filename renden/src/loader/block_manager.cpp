#include <fstream>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "loader/block_manager.hpp"
#include "loader/shader_program.hpp"

namespace world
{
BlockManager::BlockManager(const std::string& block_tex_conf, const std::string& block_def_conf)
{
	this->LoadTextures(block_tex_conf);
	this->CreateBlockPrimitives(block_def_conf);
}

void BlockManager::LoadTextures(const std::string& block_tex_conf)
{
	std::ifstream block_tex_file(block_tex_conf);
	nlohmann::json j;
	block_tex_file >> j;

	int width = j["width"];
	int height = j["height"];

	std::vector<std::string> paths;
	unsigned int idx = 0;
	for (const auto& tex_def : j["textures"].items())
	{
		const std::string& tex_name = tex_def.key();
		const std::string& tex_path = tex_def.value();
		assert(texture_name_to_layer_.find(tex_name) == texture_name_to_layer_.end());
		texture_name_to_layer_[tex_name] = idx;
		paths.push_back(PROJECT_SOURCE_DIR "/renden/res/tex/" + tex_path);
		++idx;
	}

	textures_ = std::make_unique<gl::Texture2D>(paths, width, height, 
												gl::LINEAR_MIPMAP_LINEAR, gl::NEAREST,
												gl::CLAMP_EDGE, 2, shader::BlockShader::kBlockTextureUnit);
	spdlog::info("Loaded {} textures", paths.size());
}

void BlockManager::CreateBlockPrimitives(const std::string& block_def_conf)
{
	std::ifstream block_def_file(block_def_conf);
	nlohmann::json j;
	block_def_file >> j;

	std::vector<GLubyte> str_buffer;

	int idx = 0;
	for (const auto& block_def : j)
	{
		block_id_t id = block_def["id"];
		std::string name = block_def["name"];
		std::vector<std::string> tex_names = block_def["textures"];
		std::array<char, 6 * 4 * 3> str = block_def["str"];
		bool is_opaque = block_def["opaque"];
		bool is_power_source = block_def["is_power_source"];
		bool is_conduit = block_def["is_conduit"];
		for (int i = 0; i < 6 * 4 * 3; i++)
		{
			if (i % 3 == 2)
			{
				// Assign proper r-layer, and add an additional component to fill a texel
				str_buffer.push_back(GLubyte(texture_name_to_layer_[tex_names[int(str[i])]]));
				str_buffer.push_back(GLubyte(0));
			}
			else
			{
				str_buffer.push_back(GLubyte(str[i]));
			}
		}

		const auto new_block = std::make_shared<BlockPrimitive>(idx, is_opaque, is_power_source, is_conduit);

		assert(!block_id_to_primitive_[id] && block_name_to_id_.find(name) == block_name_to_id_.end());
		block_id_to_primitive_[id] = new_block;
		block_name_to_id_[name] = id;
		++idx;
	}

	this->str_texture_ = std::make_unique<gl::Texture1D<GL_RGBA8UI,GL_RGBA_INTEGER,GL_UNSIGNED_BYTE,GLubyte>>(
		str_buffer, shader::BlockShader::kStrTextureUnit);
	spdlog::debug("loaded {} block definitions", j.size());
}


std::shared_ptr<BlockPrimitive> BlockManager::GetBlockByName(const std::string& name)
{
	auto result = block_name_to_id_.find(name);
	return result != block_name_to_id_.end()
		       ? block_id_to_primitive_[result->second]
		       : std::shared_ptr<BlockPrimitive>(nullptr);
}

std::shared_ptr<BlockPrimitive> BlockManager::GetBlockById(unsigned int id)
{
	return block_id_to_primitive_[id];
}

std::optional<unsigned int> BlockManager::GetBlockIdByName(const std::string& name)
{
	auto result = block_name_to_id_.find(name);
	return (result != block_name_to_id_.end()) ? std::optional<unsigned int>{result->second} : std::nullopt;
}
}
