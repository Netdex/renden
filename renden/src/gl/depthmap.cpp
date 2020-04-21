#include "gl/depthmap.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "control/camera.hpp"
#include "gl/shader.hpp"
#include "imgui.h"
#include "util/math.hpp"

namespace gl
{
void DepthMap::Render(const control::Camera& camera, Shader& block_shader, Shader& block_depth_shader,
                      const glm::vec3& light_dir, const std::function<void()>& renderer) const
{
	const size_t partition_count = part_intervals_.size() - 1;
	framebuffer_.Bind();
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	block_depth_shader.Activate();
	glViewport(0, 0, this->GetWidth(), this->GetHeight());
	glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_CLAMP);

	std::vector<glm::mat4> shadow_view(partition_count);
	std::vector<glm::mat4> shadow_proj(partition_count);
	std::vector<float> shadow_depth(partition_count);

	for (int i = 0; i < partition_count; ++i)
	{
		float near_plane_norm = part_intervals_[i];
		float far_plane_norm = part_intervals_[i + 1];
		ComputeShadowViewProj(camera, near_plane_norm, far_plane_norm, light_dir,
		                      shadow_view[i], shadow_proj[i], shadow_depth[i]);
		FrameBuffer::Attach(*this, GL_DEPTH_ATTACHMENT, i);
		glClear(GL_DEPTH_BUFFER_BIT);
		block_depth_shader.Bind("view", shadow_view[i]);
		block_depth_shader.Bind("proj", shadow_proj[i]);
		renderer();
	}

	FrameBuffer::Unbind();

	block_shader.Activate();
	block_shader.Bind("shadow_view", shadow_view);
	block_shader.Bind("shadow_proj", shadow_proj);
	//block_shader.Bind("shadow_depth", shadow_depth);
	block_shader.Bind("light_dir", light_dir);
}

void DepthMap::ComputeShadowViewProj(const control::Camera& camera, float near_plane_norm,
                                     float far_plane_norm,
                                     const glm::vec3& light_dir, glm::mat4& shadow_view, glm::mat4& shadow_proj,
                                     float& depth) const
{
	ImGui::Begin("Shadow View-Projection");
	auto [center, radius] = camera.ComputeFrustumBoundingSphere(near_plane_norm, far_plane_norm);
	const float texels_per_unit = float(this->GetWidth()) / (radius * 2.f);
	//const float slipping_factor = float(this->GetWidth()) / float(this->GetWidth() - 1);

	const glm::vec3 up{1, 0, 0};
	const glm::mat4 scale_mat = scale(glm::mat4{1.f}, glm::vec3{texels_per_unit});
	const glm::mat4 look_mat = lookAt(glm::vec3{}, light_dir, up);
	const glm::mat4 texel_mat = scale_mat * look_mat;
	const glm::mat4 texel_mat_inv = inverse(texel_mat);

	// Align center with texel grid
	center = glm::vec3(texel_mat_inv * floor(texel_mat * glm::vec4(center, 1.f)));

	glm::vec3 eye = center - (light_dir * radius);
	shadow_view = lookAt(eye, center, up);
	constexpr float grace = 2.f;
	shadow_proj = glm::ortho(-radius, radius, -radius, radius,
	                         -radius * grace, radius * grace);
	depth = radius * 1.f;

	ImGui::Text("Frustum Sphere: center=(%.2f %.2f %.2f), radius=%.2f", center.x, center.y, center.z, radius);
	ImGui::Text("Frustum Eye: (%.2f %.2f %.2f)", eye.x, eye.y, eye.z);
	ImGui::Separator();
	ImGui::End();
}
}
