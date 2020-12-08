#include "state.h"

state::state()
{
}

state::state(uint32_t generation, uint32_t state_flags):
	generation(generation), flags(state_flags)
{

}

bool state::check_flag(uint32_t flag)
{
	return this->flags & flag;
}

void state::set_flag(uint32_t flag)
{
	this->flags |= flag;
}

void state::reset_flag(uint32_t flag)
{
	this->flags &= ~flag;
}

uint32_t state::get_generation()
{
	return this->generation;
}

void state::set_generation(uint32_t generation)
{
	this->generation = generation;
}
