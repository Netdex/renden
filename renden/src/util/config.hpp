#ifndef RENDEN_UTIL_CONFIG_HPP_
#define RENDEN_UTIL_CONFIG_HPP_

#include <memory>

#include <toml.hpp>

namespace util::config
{
	toml::table& Get();
	bool Load();
	bool Save();
}
#endif
