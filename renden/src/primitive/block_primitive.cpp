//
// Created by netdex on 2/17/19.
//

#include <primitive/block_primitive.hpp>
#include <loader/shader_manager.hpp>

static const GLfloat CUBE_VERTICES[] = {
	// xy -z
	-0.5f, -0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f,
	// xy +z
	-0.5f, -0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f,
	-0.5f, -0.5f, 0.5f,
	// yz -x
	-0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f,
	// yz +x
	0.5f, 0.5f, 0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,
	// xz -y
	-0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,
	-0.5f, -0.5f, 0.5f,
	-0.5f, -0.5f, -0.5f,
	// xz +y
	-0.5f, 0.5f, -0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, -0.5f,
	0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, -0.5f,
	-0.5f, 0.5f, 0.5f,
};

static const GLfloat CUBE_NORMALS[] = {
	// xy -z
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	// xy +z
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	// yz -x
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	// yz +x
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	// xz -y
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	// xz +y
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
};

const block_face FACE_IDX_TO_BLOCK_FACE[] = { NEG_Z, POS_Z, NEG_X, POS_X, NEG_Y, POS_Y };

const int BLOCK_FACE_TO_IDX[] = { -1, 0, 1, -1, 2, -1, -1, -1,
								 3, -1, -1, -1, -1, -1, -1, -1,
								 4, -1, -1, -1, -1, -1, -1,
								 -1, -1, -1, -1, -1, -1, -1, -1, -1, 5 }; // I hate this
const glm::ivec3 FACE_IDX_TO_OFFSET[] = { glm::ivec3(0, 0, -1), glm::ivec3(0, 0, 1), glm::ivec3(-1, 0, 0),
										 glm::ivec3(1, 0, 0), glm::ivec3(0, -1, 0), glm::ivec3(0, 1, 0) };

block_primitive::block_primitive(std::array<float, 4*6 * 3> uv, bool is_opaque) : uv(uv), is_opaque(is_opaque) {

}


size_t block_primitive::append_vertex_list(std::vector<float> &vlist, glm::ivec3 position, block_face_set faces) {
	size_t count = vlist.size();
	for (int i = 0; i < 6; i++) {
		if (faces & FACE_IDX_TO_BLOCK_FACE[i]) {
			vlist.push_back(position.x);
			vlist.push_back(position.y);
			vlist.push_back(position.z);

			vlist.push_back(i);

			for (int j = 0; j < 4; j++) {
				vlist.insert(vlist.end(), uv.begin() + i * 4 * 3 + j * 3, uv.begin() + i * 4 * 3 + (j + 1) * 3);
			}

			//        for (int j = 0; j < 6; j++) {
			//            for (int k = 0; k < 3; k++) {
			//                vlist.push_back(CUBE_VERTICES[i * 6 * 3 + j * 3 + k] + position[k]);
			//            }
						///*for (int k = 0; k < 3; k++) {
						//	vlist.push_back(CUBE_NORMALS[i * 6 * 3 + j * 3 + k] + position[k]);
						//}*/
			//            vlist.insert(vlist.end(), uv.begin() + i * 6 * 3 + j * 3,
			//                         uv.begin() + i * 6 * 3 + (j + 1) * 3);
			//        }
		}
	}
	return vlist.size() - count;
}




