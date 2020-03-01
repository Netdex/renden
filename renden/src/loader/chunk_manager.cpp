#include <loader/chunk_manager.hpp>

namespace world::chunk
{
std::weak_ptr<chunk_mgr_t> db;

std::shared_ptr<chunk_mgr_t> load()
{
	auto chnks = std::make_shared<chunk_mgr_t>();
	db = chnks;
	return chnks;
}
}
