#ifndef RENDEN_UTIL_CONFIG_HPP_
#define RENDEN_UTIL_CONFIG_HPP_

#include <toml.hpp>

namespace util::config
{
	toml::value& Get();
	bool Load();
	bool Save();
}
#endif
