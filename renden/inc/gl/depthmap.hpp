#ifndef RENDEN_GL_DEPTHMAP_HPP_
#define RENDEN_GL_DEPTHMAP_HPP_

#include <functional>

#include "framebuffer.hpp"
#include "gl/texture.hpp"

namespace gl
{
/**
 * \brief A cascaded shadow map
 * TODO Rename this class to ShadowMap.
 */
class DepthMap : public Texture2DArray
{
public:
	/**
	 * \param width Width of depth map texture
	 * \param tex_id Texture unit used for depth map texture
	 * \param part_intervals Array of percentages representing position in frustum that a partition ends at
	 */
	DepthMap(int width, GLuint tex_id, nonstd::span<const float> part_intervals) :
		Texture2DArray(
			width, width, int(part_intervals.size()) - 1, NEAREST, NEAREST,
			CLAMP_BORDER, 1, tex_id, GL_DEPTH_COMPONENT32F),
		part_intervals_(part_intervals)
	{
		// TODO We do a double-bind on construction here.
		Bind();
		float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
		Unbind();
	}

	void Render(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& light_dir,
	            const std::function<void()>& renderer);

	void ComputeShadowViewProj(const glm::mat4& view, const glm::mat4& proj, float near_plane_norm,
	                           float far_plane_norm,
	                           const glm::vec3& light_dir, glm::mat4& shadow_view, glm::mat4& shadow_proj) const;
private:

	/**
	 * \brief Compute the bounding sphere of a truncated frustum defined by its projection matrix
	 * \param proj The projection matrix
	 * \param near_plane_norm The 0-1 normalized position along the frustum to truncate the near plane
	 * \param far_plane_norm The 0-1 normalized position along the frustum to truncate the far plane
	 * \return The bounding sphere of the frustum as (center,radius)
	 */
	static std::pair<glm::vec3, float> ComputeFrustumBoundingSphere(const glm::mat4& view, const glm::mat4& proj,
	                                                                float near_plane_norm,
	                                                                float far_plane_norm);

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

	nonstd::span<const float> part_intervals_;
	gl::FrameBuffer framebuffer_;
};
}
#endif
