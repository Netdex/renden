//
// Created by netdex on 2/17/19.
//

#include <primitive/block_primitive.hpp>
#include <loader/shader_manager.hpp>

void BlockPrimitive::AppendToVertexList(ByteBuffer<>& vlist, glm::ivec3 position, world::block::BlockFaceSet faces)
{
	for (int i = 0; i < 6; i++)
	{
		if (faces & world::block::kFaceToBlock[i])
		{
			vlist.put(value_ptr(position), 3);
			vlist.put(i);
			vlist.put(uv.data() + i * 4 * 3, 4 * 3);
		}
	}
}
