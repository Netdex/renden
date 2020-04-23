#ifndef RENDEN_UTIL_CONFIG_HPP_
#define RENDEN_UTIL_CONFIG_HPP_

#include <memory>

#include <toml++/toml.h>

namespace util::config
{
	toml::table& Get();
	bool Load();
	bool Save();
}
#endif
