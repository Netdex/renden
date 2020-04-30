#ifndef RENDEN_BLOCK_HPP
#define RENDEN_BLOCK_HPP


#include <unordered_map>

#include <glm/glm.hpp>
#include <toml.hpp>

#include "gl/texture.hpp"
#include "util/bytebuffer.hpp"

namespace world
{
enum Direction : uint_fast8_t
{
	NEG_Z = 1 << 0,
	POS_Z = 1 << 1,
	NEG_X = 1 << 2,
	POS_X = 1 << 3,
	NEG_Y = 1 << 4,
	POS_Y = 1 << 5
};

typedef uint_fast8_t DirectionMask;

constexpr Direction kDirections[] = {NEG_Z, POS_Z, NEG_X, POS_X, NEG_Y, POS_Y};

constexpr Direction ord_to_dir(int ord)
{
	return kDirections[ord];
}

constexpr Direction ord_to_dir_inv(int ord)
{
	constexpr Direction kFaceToBlockInv[] = {POS_Z, NEG_Z, POS_X, NEG_X, POS_Y, NEG_Y};
	return kFaceToBlockInv[ord];
}

constexpr int dir_to_ord(Direction dir)
{
	constexpr int kFaceToIndex[] = {
		-1, 0, 1, -1, 2, -1, -1, -1,
		3, -1, -1, -1, -1, -1, -1, -1,
		4, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, 5
	};
	return kFaceToIndex[static_cast<int>(dir)];
}

constexpr Direction dir_inv(Direction dir)
{
	return ord_to_dir_inv(dir_to_ord(dir));
}

constexpr glm::ivec3 ord_to_offset(int ord)
{
	constexpr glm::ivec3 kFaceToOffset[] = {
		glm::ivec3(0, 0, -1), glm::ivec3(0, 0, 1), glm::ivec3(-1, 0, 0),
		glm::ivec3(1, 0, 0), glm::ivec3(0, -1, 0), glm::ivec3(0, 1, 0)
	};
	return kFaceToOffset[ord];
}

constexpr glm::ivec3 dir_to_offset(Direction dir)
{
	return ord_to_offset(dir_to_ord(dir));
}

typedef int block_id_t;

class Block
{
public:

	explicit Block(block_id_t id = 0) : id_{id}
	{
	}

	bool operator==(const Block& o) const;
	bool operator!=(const Block& o) const;

	block_id_t id() const { return id_; }
	toml::table& table() const { return block_id_to_table_[id()]; }

	void AppendToVertexList(util::byte_buffer<>& vlist, glm::ivec3 position, world::DirectionMask faces) const;

	static void LoadTextures(const std::string& block_def_conf, const std::string& block_tex_conf);

	static gl::Texture2DArray& GetBlockTexture() { return *textures_; }
	static gl::Texture1D<GLubyte>& GetBlockStrTexture() { return *str_texture_; }

private:
	block_id_t id_;

	static constexpr int MAXIMUM_BLOCKS = 256;
	static toml::value block_table_;
	static std::unordered_map<block_id_t, toml::table> block_id_to_table_;


	static std::unique_ptr<gl::Texture2DArray> textures_;
	static std::unique_ptr<gl::Texture1D<GLubyte>> str_texture_;

	static std::unordered_map<std::string, block_id_t> texture_name_to_layer_;
	static std::unordered_map<std::string, block_id_t> block_name_to_id_;
};
}


#endif //RENDEN_BLOCK_HPP
