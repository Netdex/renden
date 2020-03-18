#include <glm/gtc/type_ptr.hpp>

#include "primitive/block_primitive.hpp"

void BlockPrimitive::AppendToVertexList(util::byte_buffer<>& vlist, glm::ivec3 position,
                                        world::block::BlockFaceSet faces) const
{
	for (int i = 0; i < 6; i++)
	{
		if (faces & world::block::kFaceToBlock[i])
		{
			vlist.put(value_ptr(position), 3);
			vlist.put(GLint(i));
			vlist.put(GLuint(this->TexOffset));
		}
	}
}
