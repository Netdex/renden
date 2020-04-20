#include "gl/depthmap.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "imgui.h"
#include "util/math.hpp"

namespace gl
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

void DepthMap::Render(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& light_dir,
                      const std::function<void()>& renderer)
{
	glViewport(0, 0, this->GetWidth(), this->GetHeight());
	framebuffer_.Bind();
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	for (int i = 0; i < part_intervals_.size() - 1; ++i)
	{
		float near_plane_norm = part_intervals_[i];
		float far_plane_norm = part_intervals_[i + 1];
		FrameBuffer::Attach(*this, GL_DEPTH_ATTACHMENT, i);
	}
	FrameBuffer::Unbind();
}

void DepthMap::ComputeShadowViewProj(const glm::mat4& view, const glm::mat4& proj, float near_plane_norm,
                                     float far_plane_norm,
                                     const glm::vec3& light_dir, glm::mat4& shadow_view, glm::mat4& shadow_proj) const
{
	ImGui::Begin("Shadow View-Projection");
	auto [center, radius] = ComputeFrustumBoundingSphere(view, proj, near_plane_norm, far_plane_norm);
	const float texels_per_unit = float(this->GetWidth()) / (radius * 2.f);
	const float slipping_factor = float(this->GetWidth()) / float(this->GetWidth() - 1);

	const glm::vec3 up{1, 0, 0};
	const glm::mat4 scale_mat = scale(glm::mat4{1.f}, glm::vec3{texels_per_unit});
	const glm::mat4 look_mat = lookAt(glm::vec3{}, light_dir, up);
	const glm::mat4 texel_mat = scale_mat * look_mat;
	const glm::mat4 texel_mat_inv = inverse(texel_mat);
	//const glm::mat4 texel_mat = scale_mat * view;
	//const glm::mat4 texel_mat_inv = inverse(texel_mat);

	// Align center with texel grid
	center = glm::vec3(texel_mat_inv * floor(texel_mat * glm::vec4(center, 1.f)));

	glm::vec3 eye = center - (light_dir * radius);
	shadow_view = lookAt(eye, center, up);
	constexpr float grace = 2.f;
	// TODO Depth clamping
	shadow_proj = glm::ortho(-radius, radius, -radius, radius,
	                         -radius * grace, radius * grace);

	ImGui::Text("Frustum Sphere: center=(%.2f %.2f %.2f), radius=%.2f", center.x, center.y, center.z, radius);
	ImGui::Text("Frustum Eye: (%.2f %.2f %.2f)", eye.x, eye.y, eye.z);
	ImGui::End();
}

std::pair<glm::vec3, float> DepthMap::ComputeFrustumBoundingSphere(const glm::mat4& view, const glm::mat4& proj,
                                                                   float near_plane_norm,
                                                                   float far_plane_norm)
{
	glm::mat4 aug_proj{proj};
	AugmentClipPlaneDist(aug_proj, near_plane_norm, far_plane_norm);
	glm::mat4 vp_inv = inverse(aug_proj * view);

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

std::pair<float, float> DepthMap::ComputeClipPlaneDist(const glm::mat4& proj)
{
	float c = proj[2][2];
	float d = proj[2][3];
	return std::pair{d / (c - 1.f), d / (c + 1.f)};
}

void DepthMap::AugmentClipPlaneDist(glm::mat4& proj, float near_plane_norm, float far_plane_norm)
{
	auto [clip_near, clip_far] = ComputeClipPlaneDist(proj);
	float n = util::map(near_plane_norm, 0.f, 1.f, clip_near, clip_far);
	float f = util::map(far_plane_norm, 0.f, 1.f, clip_near, clip_far);
	proj[2][2] = -(f + n) / (f - n);
	proj[2][3] = -2 * f * n / (f - n);
}
}
