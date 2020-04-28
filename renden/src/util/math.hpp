#ifndef RENDEN_UTIL_MATH_HPP_
#define RENDEN_UTIL_MATH_HPP_

#include <utility>

namespace util
{
/**
 * \brief Perform a range map of x: [a1, a2] to [b1, b2]
 * \tparam T The type of the numerical value to range map
 * \param x Value to range map
 * \param a1 Lower bound of domain
 * \param a2 Upper bound of domain
 * \param b1 Lower bound of range
 * \param b2 Upper bound of range
 * \return x mapped from domain to range
 */
template <typename T>
T map(T x, T a1, T a2, T b1, T b2)
{
	assert(a1 < a2 && b1 < b2 && a1 <= x && x <= a2);
	return b1 + (x - a1) * (b2 - b1) / (a2 - a1);
}

inline bool interval_intersects(float min_a, float max_a, float min_b, float max_b)
{
    return !((min_b < min_a && max_b < min_a) || (min_b > max_a&& max_b > max_a));
}

inline bool aabb_intersects(std::pair<glm::vec3, glm::vec3> a, std::pair<glm::vec3, glm::vec3> b)
{
	auto [min_a, max_a] = a;
	auto [min_b, max_b] = b;
    return interval_intersects(min_a.x, max_a.x, min_b.x, max_b.x)
        && interval_intersects(min_a.y, max_a.y, min_b.y, max_b.y)
        && interval_intersects(min_a.z, max_a.z, min_b.z, max_b.z);
}

}

#endif
