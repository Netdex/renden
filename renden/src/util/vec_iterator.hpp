#ifndef RENDEN_UTIL_VEC_ITERATOR_HPP_
#define RENDEN_UTIL_VEC_ITERATOR_HPP_

namespace util
{
template <typename Vec, typename T>
class vec_iterator
{
public:

	virtual ~vec_iterator()
	{
	}

	T operator++(int)
	{
		position_ += step_;
		return *this;
	}

	bool operator!=(const vec_iterator& o)
	{
		return position_ != o.position_;
	}

	virtual T operator*() = 0;

	Vec position() const { return position_; }

protected:
	vec_iterator(Vec initial, Vec step) : position_(initial), step_(step)
	{
	}

	Vec position_;
	Vec step_;
};
}
#endif
