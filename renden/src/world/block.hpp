#ifndef RENDEN_BLOCK_HPP
#define RENDEN_BLOCK_HPP

typedef unsigned char block_id_t;

class Block
{
	unsigned char power_state_;

public:
	block_id_t ID;

	explicit Block(block_id_t id = 0, unsigned char power_state = 0)
		: power_state_{power_state}, ID{id}
	{
	}

	unsigned char GetPowerState() const;
	void SetPassivePowerState(unsigned char power_state);

	bool operator==(const Block& o) const;
	bool operator!=(const Block& o) const;
};


#endif //RENDEN_BLOCK_HPP
