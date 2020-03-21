#include <glm/gtc/type_ptr.hpp>

#include "primitive/block_primitive.hpp"

void BlockPrimitive::AppendToVertexList(util::byte_buffer<>& vlist, glm::ivec3 position,
                                        world::block::BlockFaceSet faces) const
{
	vlist.put(value_ptr(position), 3);
	vlist.put(GLuint(faces));
	vlist.put(GLuint(this->TexOffset));
}
