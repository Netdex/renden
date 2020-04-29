#include "world/block.hpp"

#include <spdlog/spdlog.h>

#include "loader/shader_program.hpp"
#include "util/context.hpp"

namespace world
{
toml::value Block::block_table_;
std::unordered_map<block_id_t, toml::table> Block::block_id_to_table_;

std::unique_ptr<gl::Texture2DArray> Block::textures_;
std::unique_ptr<gl::Texture1D<GLubyte>> Block::str_texture_;

std::unordered_map<std::string, block_id_t> Block::texture_name_to_layer_;
std::unordered_map<std::string, block_id_t> Block::block_name_to_id_;

bool Block::operator==(const Block& o) const
{
	return o.id_ == id_;
}

bool Block::operator!=(const Block& o) const
{
	return !operator==(o);
}

void Block::AppendToVertexList(util::byte_buffer<>& vlist, glm::ivec3 position,
                               DirectionMask faces) const
{
	if (!id_)
		return;
	auto& table = this->table();
	vlist.put(value_ptr(position), 3);
	vlist.put(GLuint(faces));
	vlist.put(GLuint(table["tex_offset"].as_integer()));
}


void Block::LoadTextures(const std::string& block_def_conf, const std::string& block_tex_conf)
{
	try
	{
		auto tex_table = toml::parse(block_tex_conf);
		int width = toml::find<int>(tex_table, "width");
		int height = toml::find<int>(tex_table, "height");

		std::vector<std::string> paths;
		int tex_idx = 0;
		for (const auto& tex_def : tex_table["textures"].as_table())
		{
			const auto& [tex_name, tex_path] = tex_def;
			assert(texture_name_to_layer_.find(tex_name) == texture_name_to_layer_.end());
			texture_name_to_layer_[tex_name] = tex_idx;
			paths.push_back(PROJECT_SOURCE_DIR "/res/tex/" + toml::get<std::string>(tex_path));
			++tex_idx;
		}

		textures_ = std::make_unique<gl::Texture2DArray>(paths, width, height,
		                                                 gl::LINEAR_MIPMAP_LINEAR, gl::NEAREST,
		                                                 gl::CLAMP_EDGE, 2, shader::BlockShader::kBlockTextureUnit);
		spdlog::debug("Loaded {} textures", paths.size());
	}
	catch (const std::runtime_error& e)
	{
		spdlog::warn("Failed to load {}: {}", block_def_conf, e.what());
		throw;
	}

	try
	{
		block_table_ = toml::parse(block_def_conf);
		std::vector<GLubyte> str_buffer;
		int idx = 0;
		for (auto& block_def : block_table_["block"].as_array())
		{
			block_id_t id = toml::find<block_id_t>(block_def, "id");
			std::string name = toml::find<std::string>(block_def, "name");
			std::vector<std::string> tex_names = toml::find<std::vector<std::string>>(block_def, "textures");
			std::array<char, 6 * 4 * 3> str = toml::find<std::array<char, 6 * 4 * 3>>(block_def, "str");
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

			auto& def_table = block_def.as_table();
			def_table["tex_offset"] = idx;

			block_id_to_table_[id] = block_def.as_table();

			assert(block_name_to_id_.find(name) == block_name_to_id_.end());
			block_name_to_id_[name] = id;
			++idx;
		}

		str_texture_ = std::make_unique<gl::Texture1D<GLubyte>>(
			str_buffer, shader::BlockShader::kStrTextureUnit, GL_RGBA8UI,GL_RGBA_INTEGER,GL_UNSIGNED_BYTE);
		spdlog::debug("Loaded {} block definitions", block_id_to_table_.size());
	}
	catch (const std::runtime_error& e)
	{
		spdlog::warn("Failed to load {}: {}", block_def_conf, e.what());
		throw;
	}
}

}
