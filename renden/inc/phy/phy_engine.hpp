//
// Created by netdex on 21/04/19.
//

#ifndef RENDEN_PHY_ENGINE_HPP
#define RENDEN_PHY_ENGINE_HPP

#include <loader/chunk_manager.hpp>

class phy_engine {
    std::thread thread;

    bool should_run = true;

    void tick();
public:
    phy_engine(const phy_engine &o) = delete;

    phy_engine();
    ~phy_engine();

    void run();
};
#endif //RENDEN_PHY_ENGINE_HPP
