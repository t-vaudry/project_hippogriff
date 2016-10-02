#pragma once
class State
{
private:
	bool current_state;
	bool next_state;
public:
	State();
	~State();
	void changeStatus();
	bool getCurrentState();
	void setNextState(bool);
};