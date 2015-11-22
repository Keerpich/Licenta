#pragma once
#include "State.h"
#include "vector"
#include "string"

using namespace std;

class StateContainer
{
private:
	vector<State> states;

	const string filename = "p:\\Licenta\\VultureLearning\\xp.dat";
	int maxNumberOfUnits;

	//vector<State>::iterator currentState;


public:
	StateContainer(int maxNumberOfUnits);
	void LoadStates();
	void SaveStates();
	~StateContainer();

	std::vector<State>::iterator getStateByValues(bool weaponOnCooldown, DistanceToEnemy dte, int numberOfUnitsInRadius, HealthOfUnit hou);
	std::vector<State>::iterator getStateByValues(State &s);
	//void setCurrentState(bool wCooldown, DistanceToEnemy dte, int nouir, HealthOfUnit hou);
};

