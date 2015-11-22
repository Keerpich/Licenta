#include "State.h"


State::State(bool isWoc, DistanceToEnemy dToE, int noOfEIR, HealthOfUnit hp)
{
	isWeaponOnCooldown = isWoc;
	distanceToClosestEnemy = dToE;
	enemiesInRange = noOfEIR;
	agentHealth = hp;

	runStateValue = 0.f;
	fightStateValue = 0.f;
}

void State::setRunValue(float value)
{
	runStateValue = value;
}

float State::getActionValue(Action action) const
{
	return action == Action::Fight ? getFightValue() : getRunValue();
}

void State::influenceValue(Action action, float value)
{
	//inversing them because it might really be better this way
	if (action == Action::Fight)
	{
		fightStateValue += value;
	}
	else
	{
		runStateValue += value;
	}
}

void State::setFightValue(float value)
{
	fightStateValue = value;
}

float State::getRunValue() const
{
	return runStateValue;
}

float State::getFightValue() const
{
	return fightStateValue;
}

bool State::operator==(State& state)
{
	return state.isWeaponOnCooldown == isWeaponOnCooldown &&
		state.distanceToClosestEnemy == distanceToClosestEnemy &&
		state.enemiesInRange == enemiesInRange &&
		state.agentHealth == agentHealth;
}

bool State::operator==(const State& state)
{
	return state.isWeaponOnCooldown == isWeaponOnCooldown &&
		state.distanceToClosestEnemy == distanceToClosestEnemy &&
		state.enemiesInRange == enemiesInRange &&
		state.agentHealth == agentHealth;
}

Action State::getBestAction() const
{
	if (fightStateValue >= runStateValue)
		return Action::Fight;
	else
		return Action::Run;
}

State::~State()
{
}
