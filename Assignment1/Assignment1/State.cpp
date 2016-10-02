#include "State.h"

State::State()
{
	current_state = false;
	next_state = false;
}

State::~State()
{
}

void State::changeStatus()
{
	current_state = next_state;
	//next_state = false;
}

bool State::getCurrentState()
{
	return current_state;
}

void State::setNextState(bool state)
{
	next_state = state;
}