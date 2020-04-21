#ifndef RENDEN_BLOCK_TEXTURE_MANAGER_HPP
#define RENDEN_BLOCK_TEXTURE_MANAGER_HPP

#include <string>
#include <unordered_map>
#include <memory>
#include <optional>

#include "gl/texture.hpp"
#include "primitive/block_primitive.hpp"

namespace world
{
constexpr int MAXIMUM_BLOCKS = 256;

// TODO I have some problems with this class.
class BlockManager
{
	// convert name of texture to index of 2D texture array
	std::unordered_map<std::string, unsigned int> texture_name_to_layer_;
	std::unique_ptr<gl::Texture2DArray> textures_;
	std::unique_ptr<gl::Texture1D<GLubyte>> str_texture_;

	// convert name of block to block primitive mesh
	std::shared_ptr<BlockPrimitive> block_id_to_primitive_[MAXIMUM_BLOCKS];
	std::unordered_map<std::string, unsigned int> block_name_to_id_;

	void LoadTextures(const std::string& block_tex_conf);

	void CreateBlockPrimitives(const std::string& block_def_conf);

public:
	BlockManager(const std::string& block_tex_conf, const std::string& block_def_conf);

	std::shared_ptr<BlockPrimitive> GetBlockByName(const std::string& name);

	std::shared_ptr<BlockPrimitive> GetBlockById(unsigned int id);

	std::optional<unsigned int> GetBlockIdByName(const std::string& name);

	gl::Texture2DArray& GetBlockTexture() const
	{
		return *textures_;
	}

	gl::Texture1D<GLubyte>& GetBlockStrTexture() const
	{
		return *str_texture_;
	}
};
}

#endif //RENDEN_BLOCK_TEXTURE_MANAGER_HPP
