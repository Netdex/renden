#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"

camera::camera(GLFWwindow* window, float width, float height) : window(window), width(width), height(height)
{
}

camera::~camera()
{
}

void camera::update(float deltaTime, bool focus)
{
	// calculate mouse movement
	if (focus) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		glfwSetCursorPos(window, width / 2, height / 2);
		yaw += mouseSpeed * deltaTime * float(width / 2 - xpos);
		pitch += mouseSpeed * deltaTime * float(height / 2 - ypos);
		if (pitch > glm::pi<float>() / 2 - 0.01)
			pitch = static_cast<float>(glm::pi<float>() / 2 - 0.01);
		if (pitch < -glm::pi<float>() / 2 + 0.01)
			pitch = static_cast<float>(-glm::pi<float>() / 2 + 0.01);
	}

	// calculate camera vectors
	glm::vec3 direction = normalize(this->get_direction());
	glm::vec3 right = normalize(cross(direction, up));

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		speed = 100.0f;
	}
	else {
		speed = 10.0f;
	}

	// handle input
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position += direction * deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position -= direction * deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += right * deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position -= right * deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		position -= glm::vec3(0, 1, 0) * deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		position += glm::vec3(0, 1, 0) * deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
		if (fov > 0.01)
			fov -= 0.01;
	}
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
		if (fov < glm::pi<float>() - 0.01)
			fov += 0.01;
	}

	view = glm::lookAt(
		position,
		position + direction,
		up
	);
	proj = glm::perspective(fov, width / height, 0.01f, 1000.0f);
}

std::optional<std::pair<glm::ivec3, glm::ivec3>> camera::cast_target(world::chunk::chunk_mgr_t & cnk_mgr, int distance)
{
	assert(distance > 0);

	glm::vec3 u = position;
	glm::vec3 v = normalize(this->get_direction());

	int x = int(roundf(u.x));
	int y = int(roundf(u.y));
	int z = int(roundf(u.z));

	int step_x = v.x > 0 ? 1 : -1;
	int step_y = v.y > 0 ? 1 : -1;
	int step_z = v.z > 0 ? 1 : -1;

	float t_max_x = (step_x > 0 ? ceilf(u.x - 0.5f) + 0.5f - u.x 
		: u.x - floorf(u.x + 0.5f) + 0.5f) / fabs(v.x);
	float t_max_y = (step_y > 0 ? ceilf(u.y - 0.5f) + 0.5f - u.y 
		: u.y - floorf(u.y + 0.5f) + 0.5f) / fabs(v.y);
	float t_max_z = (step_z > 0 ? ceilf(u.z - 0.5f) + 0.5f - u.z 
		: u.z - floorf(u.z + 0.5f) + 0.5f) / fabs(v.z);

	float t_delta_x = step_x / v.x;
	float t_delta_y = step_y / v.y;
	float t_delta_z = step_z / v.z;

	glm::ivec3 norm;

	for (int d = 0; d < distance; d++) {
		if (t_max_x < t_max_y) {
			if (t_max_x < t_max_z) {
				x = x + step_x;
				t_max_x = t_max_x + t_delta_x;
				norm = glm::ivec3(-step_x, 0, 0);
			}
			else {
				z = z + step_z;
				t_max_z = t_max_z + t_delta_z;
				norm = glm::ivec3(0, 0, -step_z);
			}
		}
		else {
			if (t_max_y < t_max_z) {
				y = y + step_y;
				t_max_y = t_max_y + t_delta_y;
				norm = glm::ivec3(0, -step_y, 0);
			}
			else {
				z = z + step_z;
				t_max_z = t_max_z + t_delta_z;
				norm = glm::ivec3(0, 0, -step_z);
			}
		}
		glm::ivec3 pos = glm::ivec3(x, y, z);
		std::optional<block> blk = cnk_mgr.get_block_at(pos);
		if (blk && blk->id > 0) return std::make_pair(pos, norm);
	}
	return std::nullopt;
}

glm::vec3 camera::get_direction() const
{
	return glm::vec3(
		std::cos(pitch) * std::sin(yaw),
		std::sin(pitch),
		std::cos(pitch) * std::cos(yaw)
	);
}
