#include "control/camera.hpp"

#include <cmath>

#include <glm/gtc/matrix_transform.hpp>

#include "world/world.hpp"

namespace control
{
Camera::Camera(GLFWwindow* window) : window_(window)
{
}

Camera::~Camera() = default;

void Camera::Update(float deltaTime, bool focus)
{
	int width, height;
	glfwGetFramebufferSize(window_, &width, &height);
	// calculate mouse movement
	if (focus)
	{
		double xpos, ypos;
		glfwGetCursorPos(window_, &xpos, &ypos);
		glfwSetCursorPos(window_, width / 2, height / 2);
		yaw_ += mouse_speed_ * deltaTime * float(width / 2 - xpos);
		pitch_ += mouse_speed_ * deltaTime * float(height / 2 - ypos);
		if (pitch_ > glm::pi<float>() / 2 - 0.01)
			pitch_ = static_cast<float>(glm::pi<float>() / 2 - 0.01);
		if (pitch_ < -glm::pi<float>() / 2 + 0.01)
			pitch_ = static_cast<float>(-glm::pi<float>() / 2 + 0.01);
	}

	// calculate camera vectors
	const glm::vec3 direction = normalize(this->GetDirection());
	const glm::vec3 right = normalize(cross(direction, up_));

	if (glfwGetKey(window_, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed_ = 100.0f;
	}
	else
	{
		speed_ = 10.0f;
	}

	// handle input
	if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
	{
		Position += direction * deltaTime * speed_;
	}
	if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
	{
		Position -= direction * deltaTime * speed_;
	}
	if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += right * deltaTime * speed_;
	}
	if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position -= right * deltaTime * speed_;
	}
	if (glfwGetKey(window_, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		Position -= glm::vec3(0, 1, 0) * deltaTime * speed_;
	}
	if (glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		Position += glm::vec3(0, 1, 0) * deltaTime * speed_;
	}
	if (glfwGetKey(window_, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
	{
		if (fov_ > 0.01)
			fov_ -= 0.01;
	}
	if (glfwGetKey(window_, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
	{
		if (fov_ < glm::pi<float>() - 0.01)
			fov_ += 0.01;
	}

	View = glm::lookAt(
		Position,
		Position + direction,
		up_
	);
	Proj = glm::perspective(fov_, float(width) / height, 0.01f, 1000.0f);
}

std::optional<std::pair<glm::ivec3, glm::ivec3>> Camera::CastTarget(world::World& world,
                                                                    int distance) const
{
	assert(distance > 0);

	glm::vec3 u = Position;
	glm::vec3 v = normalize(this->GetDirection());

	int x = int(roundf(u.x));
	int y = int(roundf(u.y));
	int z = int(roundf(u.z));

	int step_x = v.x > 0 ? 1 : -1;
	int step_y = v.y > 0 ? 1 : -1;
	int step_z = v.z > 0 ? 1 : -1;

	float t_max_x = (step_x > 0
		                 ? ceilf(u.x - 0.5f) + 0.5f - u.x
		                 : u.x - floorf(u.x + 0.5f) + 0.5f) / fabs(v.x);
	float t_max_y = (step_y > 0
		                 ? ceilf(u.y - 0.5f) + 0.5f - u.y
		                 : u.y - floorf(u.y + 0.5f) + 0.5f) / fabs(v.y);
	float t_max_z = (step_z > 0
		                 ? ceilf(u.z - 0.5f) + 0.5f - u.z
		                 : u.z - floorf(u.z + 0.5f) + 0.5f) / fabs(v.z);

	float t_delta_x = float(step_x) / v.x;
	float t_delta_y = float(step_y) / v.y;
	float t_delta_z = float(step_z) / v.z;

	glm::ivec3 norm;

	for (int d = 0; d < distance; d++)
	{
		if (t_max_x < t_max_y)
		{
			if (t_max_x < t_max_z)
			{
				x = x + step_x;
				t_max_x = t_max_x + t_delta_x;
				norm = glm::ivec3(-step_x, 0, 0);
			}
			else
			{
				z = z + step_z;
				t_max_z = t_max_z + t_delta_z;
				norm = glm::ivec3(0, 0, -step_z);
			}
		}
		else
		{
			if (t_max_y < t_max_z)
			{
				y = y + step_y;
				t_max_y = t_max_y + t_delta_y;
				norm = glm::ivec3(0, -step_y, 0);
			}
			else
			{
				z = z + step_z;
				t_max_z = t_max_z + t_delta_z;
				norm = glm::ivec3(0, 0, -step_z);
			}
		}
		glm::ivec3 pos = glm::ivec3(x, y, z);
		std::optional<Block> blk = world.GetBlockAt(pos);
		if (blk && blk->ID > 0) return std::make_pair(pos, norm);
	}
	return std::nullopt;
}

glm::vec3 Camera::GetDirection() const
{
	return glm::vec3(
		std::cos(pitch_) * std::sin(yaw_),
		std::sin(pitch_),
		std::cos(pitch_) * std::cos(yaw_)
	);
}
}
