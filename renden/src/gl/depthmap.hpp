#ifndef RENDEN_GL_DEPTHMAP_HPP_
#define RENDEN_GL_DEPTHMAP_HPP_

#include <functional>

#include "control/camera.hpp"
#include "gl/framebuffer.hpp"
#include "gl/shader.hpp"
#include "gl/texture.hpp"

namespace control
{
}

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
			width, width, int(part_intervals.size()) - 1,NEAREST,NEAREST,
			CLAMP_BORDER, 1, tex_id, GL_DEPTH_COMPONENT24),
		part_intervals_(part_intervals)
	{
		// TODO We do a double-bind on construction here.
		Bind();
		SetBorderColor({ 1.0f,1.0f,1.0f,1.0f });
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
		Unbind();
	}

	void Render(const control::Camera& camera, Shader& block_shader, Shader& block_depth_shader,
	            const glm::vec3& light_dir, const std::function<void()>& renderer) const;

private:
	void ComputeShadowViewProj(const control::Camera& camera, float near_plane_norm,
	                           float far_plane_norm,
	                           const glm::vec3& light_dir, glm::mat4& shadow_view,
	                           glm::mat4& shadow_proj, float& depth) const;


	nonstd::span<const float> part_intervals_;
	gl::FrameBuffer framebuffer_;
};
}
#endif
