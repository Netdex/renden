//
// Created by netdex on 2/17/19.
//

#ifndef RENDEN_BLOCK_HPP
#define RENDEN_BLOCK_HPP


class block {
    unsigned char power_state;

public:
    unsigned char id;

    explicit block(unsigned char id = 0, unsigned char power_state = 0)
            : id{id}, power_state{power_state} {}

    unsigned char get_power_state();
    void set_passive_power_state(unsigned char power_state);

    bool operator==(const block &o);
    bool operator!=(const block &o);
};


#endif //RENDEN_BLOCK_HPP
