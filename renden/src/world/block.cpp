//
// Created by netdex on 2/17/19.
//

#include <world/block.hpp>
#include <loader/block_manager.hpp>
#include <climits>

#include "world/block.hpp"
#include "util/context.hpp"

unsigned char Block::GetPowerState() const
{
	auto& block_db = Context<world::BlockManager>::Get();

		const auto block = block_db.GetBlockById(this->ID);
	if (block && block->IsPowerSource)
		return 16;
	return power_state_;
}

void Block::SetPassivePowerState(unsigned char power_state)
{
	this->power_state_ = power_state;
}

bool Block::operator==(const Block& o) const
{
	return o.ID == ID && o.power_state_ == power_state_;
}

bool Block::operator!=(const Block& o) const
{
	return !operator==(o);
}
