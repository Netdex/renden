#ifndef RENDEN_BLOCK_PRIMITIVE_HPP
#define RENDEN_BLOCK_PRIMITIVE_HPP

#include <array>

#include <glm/vec3.hpp>

#include "util/bytebuffer.hpp"
#include "world/block.hpp"


namespace world::block
{
enum BlockFace : int
{
	NEG_Z = 1 << 0,
	POS_Z = 1 << 1,
	NEG_X = 1 << 2,
	POS_X = 1 << 3,
	NEG_Y = 1 << 4,
	POS_Y = 1 << 5
};

typedef int BlockFaceSet;

constexpr BlockFace kFaceToBlock[] = {NEG_Z, POS_Z, NEG_X, POS_X, NEG_Y, POS_Y};

constexpr int kFaceToIndex[] = {
	-1, 0, 1, -1, 2, -1, -1, -1,
	3, -1, -1, -1, -1, -1, -1, -1,
	4, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, 5
}; // I hate this

constexpr glm::ivec3 kFaceToOffset[] = {
	glm::ivec3(0, 0, -1), glm::ivec3(0, 0, 1), glm::ivec3(-1, 0, 0),
	glm::ivec3(1, 0, 0), glm::ivec3(0, -1, 0), glm::ivec3(0, 1, 0)
};
}

// TODO I don't like this class, could probably combine it with Block.
class BlockPrimitive
{
public:
	int TexOffset;
	bool IsOpaque;
	bool IsPowerSource;
	bool IsConduit;

	explicit BlockPrimitive(int tex_offset,
	                        bool is_opaque = false,
	                        bool is_power_source = false,
	                        bool is_conduit = false)
		: TexOffset{tex_offset},
		  IsOpaque{is_opaque},
		  IsPowerSource{is_power_source},
		  IsConduit{is_conduit}
	{
	}

	void AppendToVertexList(util::byte_buffer<>& vlist, glm::ivec3 position, world::block::BlockFaceSet faces) const;
};


#endif //RENDEN_BLOCK_PRIMITIVE_HPP
