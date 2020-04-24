#include "world/block.hpp"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "loader/shader_program.hpp"
#include "util/context.hpp"

namespace world
{
std::unique_ptr<gl::Texture2DArray> Block::textures_;
std::unique_ptr<gl::Texture1D<GLubyte>> Block::str_texture_;

std::unordered_map<std::string, block_id_t> Block::texture_name_to_layer_;
std::unordered_map<std::string, block_id_t> Block::block_name_to_id_;
std::unordered_map<block_id_t, int> Block::block_id_to_tex_offset_;

bool Block::operator==(const Block& o) const
{
	return o.id_ == id_;
}

bool Block::operator!=(const Block& o) const
{
	return !operator==(o);
}

void Block::AppendToVertexList(util::byte_buffer<>& vlist, glm::ivec3 position,
                               block_face_mask_t faces) const
{
	if (!id_)
		return;
	vlist.put(value_ptr(position), 3);
	vlist.put(GLuint(faces));
	vlist.put(GLuint(block_id_to_tex_offset_[this->id()]));
}


void Block::LoadTextures(const std::string& block_def_conf, const std::string& block_tex_conf)
{
	//block_table_ = toml::parse_file(block_tex_conf);
	//int width = int(block_table_["width"].ref<int64_t>());
	std::ifstream block_tex_file(block_tex_conf);

	nlohmann::json tex_def_json;
	block_tex_file >> tex_def_json;

	int width = tex_def_json["width"];
	int height = tex_def_json["height"];

	std::vector<std::string> paths;
	int tex_idx = 0;
	for (const auto& tex_def : tex_def_json["textures"].items())
	{
		const std::string& tex_name = tex_def.key();
		const std::string& tex_path = tex_def.value();
		assert(texture_name_to_layer_.find(tex_name) == texture_name_to_layer_.end());
		texture_name_to_layer_[tex_name] = tex_idx;
		paths.push_back(PROJECT_SOURCE_DIR "/renden/res/tex/" + tex_path);
		++tex_idx;
	}

	textures_ = std::make_unique<gl::Texture2DArray>(paths, width, height,
	                                                 gl::LINEAR_MIPMAP_LINEAR, gl::NEAREST,
	                                                 gl::CLAMP_EDGE, 2, shader::BlockShader::kBlockTextureUnit);
	spdlog::debug("Loaded {} textures", paths.size());

	std::ifstream block_def_file(block_def_conf);
	nlohmann::json block_def_json;
	block_def_file >> block_def_json;

	std::vector<GLubyte> str_buffer;

	int idx = 0;
	for (const auto& block_def : block_def_json)
	{
		block_id_t id = block_def["id"];
		std::string name = block_def["name"];
		std::vector<std::string> tex_names = block_def["textures"];
		std::array<char, 6 * 4 * 3> str = block_def["str"];
		//bool is_opaque = block_def["opaque"];
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

		block_id_to_tex_offset_[id] = idx;

		assert(block_name_to_id_.find(name) == block_name_to_id_.end());
		block_name_to_id_[name] = id;
		++idx;
	}

	str_texture_ = std::make_unique<gl::Texture1D<GLubyte>>(
		str_buffer, shader::BlockShader::kStrTextureUnit, GL_RGBA8UI,GL_RGBA_INTEGER,GL_UNSIGNED_BYTE);
	spdlog::debug("Loaded {} block definitions", block_def_json.size());
}
}
