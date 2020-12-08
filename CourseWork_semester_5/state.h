#pragma once
#include <stdint.h>

enum states {
	ADD = 2,
	REMOVE = 4,
	UNUSED = 8,
};


class state
{
private:
	uint32_t generation = 0;
	uint32_t flags = 0;
public:
	explicit state();
	explicit state(uint32_t generation, uint32_t state_flags);

	bool check_flag(uint32_t flag);
	void set_flag(uint32_t flag);
	void reset_flag(uint32_t flag);
	uint32_t get_generation();
	void set_generation(uint32_t generation);
};

