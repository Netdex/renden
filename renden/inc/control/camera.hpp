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
	GLFWwindow* window_ = nullptr;

	float fov_ = glm::radians(45.0f);
	float yaw_ = glm::pi<float>(); // horizontal angle : toward -Z
	float pitch_ = 0.0f; // vertical angle : 0, look at the horizon
	float speed_ = 10.0f;
	float mouse_speed_ = 0.1f;
	glm::vec3 up_ = glm::vec3(0, 1, 0);

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

	float GetAspectRatio() const;
	float GetFov() const;
};
}


#endif //RENDEN_CAMERA_HPP
