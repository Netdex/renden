#include "control/camera.hpp"

#include <cmath>

#include <glm/gtc/matrix_transform.hpp>

#include "util/math.hpp"
#include "world/world.hpp"

namespace control
{
namespace
{
	constexpr glm::vec4 kNormalizedFrustumBounds[] = {
		glm::vec4{-1, -1, -1, 1},
		glm::vec4{1, -1, -1, 1},
		glm::vec4{1, 1, -1, 1},
		glm::vec4{-1, 1, -1, 1},
		glm::vec4{-1, -1, 1, 1},
		glm::vec4{1, -1, 1, 1},
		glm::vec4{1, 1, 1, 1},
		glm::vec4{-1, 1, 1, 1}
	};
}

Camera::Camera(GLFWwindow* window) : window_(window)
{
}

Camera::~Camera() = default;

void Camera::Update(double deltaTime, bool focus)
{
	int width, height;
	glfwGetFramebufferSize(window_, &width, &height);
	// calculate mouse movement
	if (focus)
	{
		double xpos, ypos;
		glfwGetCursorPos(window_, &xpos, &ypos);
		glfwSetCursorPos(window_, width / 2, height / 2);
		const double movement = mouse_speed_ * deltaTime;
		yaw_ += movement * (width / 2 - xpos);
		pitch_ += movement * (height / 2 - ypos);
		pitch_ = glm::clamp(pitch_, -glm::pi<double>() / 2 + 0.01, glm::pi<double>() / 2 - 0.01);
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

	const auto movement = float(deltaTime * speed_);
	// handle input
	if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
	{
		Position += direction * movement;
	}
	if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
	{
		Position -= direction * movement;
	}
	if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += right * movement;
	}
	if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position -= right * movement;
	}
	if (glfwGetKey(window_, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		Position -= glm::vec3(0, 1, 0) * movement;
	}
	if (glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		Position += glm::vec3(0, 1, 0) * movement;
	}
	if (glfwGetKey(window_, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
	{
		if (fov_ > 0.01f)
			fov_ -= 0.01f;
	}
	if (glfwGetKey(window_, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
	{
		if (fov_ < glm::pi<float>() - 0.01f)
			fov_ += 0.01f;
	}

	View = lookAt(
		Position,
		Position + direction,
		up_
	);
	Proj = glm::perspective(fov_, float(width) / float(height), 0.01f, 200.0f);
}

std::optional<std::pair<glm::ivec3, glm::ivec3>> Camera::CastTarget(world::World& world,
                                                                    int distance) const
{
	assert(distance > 0);

	glm::vec3 u = Position;
	glm::vec3 v = normalize(this->GetDirection());

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
	auto it = world.cbegin(round(u));

	for (int d = 0; d < distance; d++)
	{
		if (t_max_x < t_max_y)
		{
			if (t_max_x < t_max_z)
			{
				if (step_x > 0) ++it.x;
				else --it.x;
				t_max_x = t_max_x + t_delta_x;
				norm = glm::ivec3(-step_x, 0, 0);
			}
			else
			{
				if (step_z > 0) ++it.z;
				else --it.z;
				t_max_z = t_max_z + t_delta_z;
				norm = glm::ivec3(0, 0, -step_z);
			}
		}
		else
		{
			if (t_max_y < t_max_z)
			{
				if (step_y > 0) ++it.y;
				else --it.y;
				t_max_y = t_max_y + t_delta_y;
				norm = glm::ivec3(0, -step_y, 0);
			}
			else
			{
				if (step_z > 0) ++it.z;
				else --it.z;
				t_max_z = t_max_z + t_delta_z;
				norm = glm::ivec3(0, 0, -step_z);
			}
		}
		auto blk = *it;
		if (blk && blk->id()) return std::make_pair(it.position(), norm);
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

std::pair<glm::vec3, float> Camera::ComputeFrustumBoundingSphere(float near_plane_norm, float far_plane_norm) const
{
	glm::mat4 aug_proj{Proj};
	AugmentClipPlaneDist(aug_proj, near_plane_norm, far_plane_norm);
	glm::mat4 vp_inv = inverse(aug_proj * View);

	glm::vec3 worldFrustumBounds[8];

	glm::vec3 center{};
	for (int i = 0; i < 8; ++i)
	{
		const glm::vec4& corner = kNormalizedFrustumBounds[i];
		glm::vec4 transform(vp_inv * corner);
		const glm::vec3 world = transform / transform.w;
		worldFrustumBounds[i] = world;
		center += world;
	}
	float radius = distance(worldFrustumBounds[6], worldFrustumBounds[0]) / 2.f;
	center /= 8;
	return {center, radius};
}

std::pair<glm::vec3, glm::vec3> Camera::ComputeFrustumAABB() const
{
	glm::vec3 min{FLT_MAX};
	glm::vec3 max{-min};

	glm::mat4 vp_inv = inverse(Proj * View);
	for (int i = 0; i < 8; ++i)
	{
		const glm::vec4& corner = kNormalizedFrustumBounds[i];
		glm::vec4 transform(vp_inv * corner);
		const glm::vec3 world = transform / transform.w;
		max = glm::max(max, world);
		min = glm::min(min, world);
	}
	return {min, max};
}

std::pair<float, float> Camera::ComputeClipPlaneDist(const glm::mat4& proj)
{
	float c = proj[2][2];
	float d = proj[3][2];
	return std::pair{d / (c - 1.f), d / (c + 1.f)};
}

void Camera::AugmentClipPlaneDist(glm::mat4& proj, float near_plane_norm, float far_plane_norm)
{
	auto [clip_near, clip_far] = ComputeClipPlaneDist(proj);
	float n = util::map(near_plane_norm, 0.f, 1.f, clip_near, clip_far);
	float f = util::map(far_plane_norm, 0.f, 1.f, clip_near, clip_far);
	proj[2][2] = -(f + n) / (f - n);
	proj[3][2] = -2.f * f * n / (f - n);
}
}
