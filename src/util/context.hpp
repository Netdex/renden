#pragma once

#include <memory>
#include <cassert>

/*
 * Lazy singleton with defined lifetime, from first get to first reset.
 * Mainly used for the convenience of globals, without 
 */
template <typename T>
class Context
{
	static std::unique_ptr<T> instance_;
public:
	template <typename... Args>
	static T& Initialize(Args&&... args)
	{
		assert(!instance_);
		instance_ = std::make_unique<T>(std::forward<Args>(args)...);
		return *instance_;
	}

	static T& Get()
	{
		assert(instance_);
		return *instance_;
	}

	static void Reset()
	{
		assert(instance_);
		instance_.reset();
	}
};

template<typename T> std::unique_ptr<T> Context<T>::instance_;
