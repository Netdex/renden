#include "util/config.hpp"

#include <fstream>

#include <spdlog/spdlog.h>

namespace util::config
{
namespace
{
	const std::string kConfigFilePath = "renden.toml";

	toml::table table;
}


toml::table& Get()
{
	return table;
}

bool Load()
{
	try
	{
		table = toml::parse_file(kConfigFilePath);
		return true;
	}
	catch (const toml::parse_error& e)
	{
		spdlog::warn("Failed to load {}: {}", kConfigFilePath, e.what());
		return false;
	}
}

bool Save()
{
	std::ofstream of{kConfigFilePath};
	if (!of.good())
		return false;
	of << table;
	return of.good();
}
}
