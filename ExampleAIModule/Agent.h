#pragma once
#include <vector>
#include <list>
#include <limits>
#include "StateContainer.h"
#include "BWAPI.h"
#include "BWAPI\Client.h"
#include "Vector2.h"

class Agent
{
private:
	struct Mine
	{
		int mineId;
		std::vector<State>::iterator deployedInState;
	};

	vector<Mine> allMines;

	std::vector<State>::iterator currentState;
	std::list<BWAPI::Unit> spiderMines;

	bool waitingForMine;

	StateContainer *state_container;
	BWAPI::Unit thisUnit;

	int framesPassed;
	int framesNeeded;

	float explorationEpsilon;
	float learningFactor;
	float gammaFactor;

	Action currentAction;

	int lastHpDifference;

	void Agent::GetAllSpiderMines(BWAPI::Unitset &spiderMines);
	void influenceStateByMineExplosion();

	bool canUseMineNow();

	DistanceToEnemy getDistanceToClosestEnemy();

	BWAPI::Unit getClosestEnemy();

	HealthOfUnit getHealthStatus();
	bool isWeaponOnCooldown();
	int numberOfEnemiesInRange();
	BWAPI::Unitset getEnemiesInRange();

	bool hasAnyMinesNotAdded();

	Action decideOnAction();
	int currentReward();

	void TakeAction(Action action);

	void Attack();
	void Deploy();
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

