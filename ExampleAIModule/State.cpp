#include "State.h"


State::State(bool isWoc, DistanceToEnemy dToE, int noOfEIR, HealthOfUnit hp)
{
	isWeaponOnCooldown = isWoc;
	distanceToClosestEnemy = dToE;
	enemiesInRange = noOfEIR;
	agentHealth = hp;

	runStateValue = 0.f;
	fightEnemyStateValue = 0.f;
	fightAllyStateValue = 0.f;
}

float State::getActionValue(Action action) const
{
	switch (action)
	{
	case FightEnemy:
		return fightEnemyStateValue;
		break;
	case FightAlly:
		return fightAllyStateValue;
		break;
	case Run:
		return runStateValue;
		break;
	}
	//return action == Action::Fight ? getFightValue() : getRunValue();
}

void State::influenceValue(Action action, float value)
{
	//inversing them because it might really be better this way
	if (action == Action::FightEnemy)
	{
		fightEnemyStateValue += value;
	}
	else if (action == Action::FightAlly)
	{
		fightAllyStateValue += value;
	}
	else
	{
		runStateValue += value;
	}
}

void State::setRunValue(float value)
{
	runStateValue = value;
}

void State::setFightEnemyValue(float value)
{
	fightEnemyStateValue = value;
}

void State::setFightAllyValue(float value)
{
	fightAllyStateValue = value;
}

float State::getRunValue() const
{
	return runStateValue;
}

float State::getFightEnemyValue() const
{
	return fightEnemyStateValue;
}

float State::getFightAllyValue() const
{
	return fightAllyStateValue;
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
	if (fightEnemyStateValue >= fightAllyStateValue && fightEnemyStateValue >= runStateValue)
		return Action::FightEnemy;
	else if (fightAllyStateValue >= fightEnemyStateValue && fightAllyStateValue >= runStateValue)
		return Action::FightAlly;
	else
		return Action::Run;
}

State::~State()
{
}
