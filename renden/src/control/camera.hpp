#ifndef RENDEN_CAMERA_HPP
#define RENDEN_CAMERA_HPP

#include <optional>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>

namespace world
{
	class World;
}

namespace control
{
class Camera
{
public:
	glm::vec3 Position = glm::vec3(0, 0, 5);
	glm::mat4 View, Proj;

	Camera(GLFWwindow* window);
	~Camera();

	Camera(const Camera& o) = delete;
	Camera& operator=(const Camera& o) = delete;

	void Update(float deltaTime, bool focus);
	std::optional<std::pair<glm::ivec3, glm::ivec3>> CastTarget(world::World& world, int distance) const;
	glm::vec3 GetDirection() const;

	/**
	 * \brief Compute the bounding sphere of a truncated frustum defined by its projection matrix
	 * \param near_plane_norm The 0-1 normalized position along the frustum to truncate the near plane
	 * \param far_plane_norm The 0-1 normalized position along the frustum to truncate the far plane
	 * \return The bounding sphere of the frustum as (center,radius)
	 */
	std::pair<glm::vec3, float> ComputeFrustumBoundingSphere( float near_plane_norm, float far_plane_norm) const;

	/**
	 * \brief Compute the axis-aligned bounding box of the camera's frustum
	 * \return The axis-aligned bounding box of the camera's frustum
	 */
	std::pair<glm::vec3, glm::vec3> ComputeFrustumAABB() const;
private:

	/**
	 * \brief Compute the distance to the near and far clipping planes from a projection matrix.
	 * \param proj A projection matrix
	 * \return A pair of floats of the form (near, far)
	 */
	static std::pair<float, float> ComputeClipPlaneDist(const glm::mat4& proj);

	/**
	 * \brief Modify the near and far plane distance of a projection matrix
	 * \param proj The projection matrix to modify
	 * \param near_plane_norm The new position of the near plane as a normalized value from [0,1]
	 * as a fraction of the original frustum range
	 * \param far_plane_norm The new position of the far plane as a normalized value from [0,1]
	 * as a fraction of the original frustum range
	 */
	static void AugmentClipPlaneDist(glm::mat4& proj, float near_plane_norm, float far_plane_norm);


	GLFWwindow* window_ = nullptr;

	float fov_ = glm::radians(45.0f);
	float yaw_ = glm::pi<float>(); // horizontal angle : toward -Z
	float pitch_ = 0.0f; // vertical angle : 0, look at the horizon
	float speed_ = 10.0f;
	float mouse_speed_ = 0.1f;
	glm::vec3 up_ = glm::vec3(0, 1, 0);

};
}


#endif //RENDEN_CAMERA_HPP
