//
// Created by netdex on 2/17/19.
//

#include <world/block.hpp>
#include <loader/block_manager.hpp>
#include <climits>

#include "world/block.hpp"

unsigned char block::get_power_state() {
    auto block_db = world::entities::blocks::db.lock();

    auto block = block_db->get_block_by_id(this->id);
    if(block && block->is_power_source)
        return 16;
    return power_state;
}

void block::set_passive_power_state(unsigned char power_state) {
    this->power_state = power_state;
}

bool block::operator==(const block &o) {
    return o.id == id && o.power_state == power_state;
}

bool block::operator!=(const block &o) {
    return !operator==(o);
}

