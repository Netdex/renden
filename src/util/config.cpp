#include "util/config.hpp"

#include <fstream>

#include <spdlog/spdlog.h>

namespace util::config
{
namespace
{
	const std::string kConfigFilePath = "renden.toml";

	toml::value table;
}


toml::value& Get()
{
	return table;
}

bool Load()
{
	try
	{
		table = toml::parse(kConfigFilePath);
		return true;
	}
	catch (const std::runtime_error& e)
	{
		spdlog::warn("Failed to load {}: {}", kConfigFilePath, e.what());
		table = toml::table{};
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
