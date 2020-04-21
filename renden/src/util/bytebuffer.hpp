#ifndef RENDEN_BYTEBUFFER_HPP
#define RENDEN_BYTEBUFFER_HPP

#include <vector>

#include <glad/glad.h>
#include <nonstd/span.hpp>

namespace util
{
template <typename B = GLbyte>
class byte_buffer
{
	std::vector<B> buffer_;
public:
	byte_buffer()
	{
	}

	operator nonstd::span<B>()
	{
		return buffer_;
	}

	template <typename T>
	void put(const T& var)
	{
		auto ptr = reinterpret_cast<const B*>(&var);
		buffer_.insert(buffer_.end(), ptr, ptr + sizeof(T));
	}

	template <typename T>
	void put(T* data, size_t size)
	{
		auto ptr = reinterpret_cast<const B*>(data);
		buffer_.insert(buffer_.end(), ptr, ptr + size * sizeof(T));
	}

	const B* data() { return buffer_.data(); }

	size_t size() { return buffer_.size(); }
};
}

#endif //RENDEN_BYTEBUFFER_HPP
