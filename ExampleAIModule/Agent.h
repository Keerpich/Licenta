#pragma once
#include <vector>
#include "StateContainer.h"
#include "BWAPI.h"
#include "BWAPI\Client.h"
#include "Vector2.h"


class Agent
{
private:
	std::vector<State>::iterator currentState;

	StateContainer *state_container;
	BWAPI::Unit thisUnit;

	int framesPassed;
	int framesNeeded;

	float explorationEpsilon;
	float learningFactor;
	float gammaFactor;

	Action currentAction;

	int lastHpDifference;

	DistanceToEnemy getDistanceToClosestEnemy();

	BWAPI::Unit getClosestEnemy();
	BWAPI::Unit getClosestAlly();

	HealthOfUnit getHealthStatus();
	bool isWeaponOnCooldown();
	int numberOfEnemiesInRange();
	BWAPI::Unitset getEnemiesInRange();
	BWAPI::Unitset getAlliesInRange();

	Action decideOnAction();
	int currentReward();

	void TakeAction(Action action);

	void AttackAlly();
	void AttackEnemy();
	void Flee();
public:
	void Update();
	bool isInitialized();

	Agent& operator=(Agent &other);

	Agent();
	Agent(StateContainer *container, BWAPI::Unit unit);
	Agent(StateContainer *container, BWAPI::Unit unit, float epsilon);
	~Agent();
};

