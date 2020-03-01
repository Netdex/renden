#ifndef RENDEN_BLOCK_HPP
#define RENDEN_BLOCK_HPP

class Block
{
	unsigned char power_state_;

public:
	unsigned char Id;

	explicit Block(unsigned char id = 0, unsigned char power_state = 0)
		: power_state_{power_state}, Id{id}
	{
	}

	unsigned char GetPowerState() const;
	void SetPassivePowerState(unsigned char power_state);

	bool operator==(const Block& o) const;
	bool operator!=(const Block& o) const;
};


#endif //RENDEN_BLOCK_HPP
