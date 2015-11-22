#include "Agent.h"
#include <ctime>
#include <fstream>

///<summary>Initializes the agent and it's starting state</summary>
///<param name="container">The state container for this type of unit</param>
///<param name="unit">The unit that will be handled by this agent</param>
Agent::Agent(StateContainer *container, BWAPI::Unit unit)
{
	thisUnit = unit;
	state_container = container;
	currentState = state_container->getStateByValues(isWeaponOnCooldown(), getDistanceToClosestEnemy(), numberOfEnemiesInRange(), getHealthStatus());

	BWAPI::Unitset allEnemies = BWAPI::Broodwar->enemy()->getUnits();

	int enemiesHp = 0;
	currentAction = Action::Fight;
	gammaFactor = 0.9f;
	learningFactor = 0.05f;
	explorationEpsilon = 100.f;

	srand(time(NULL));

	for (auto& e : allEnemies)
	{
		enemiesHp += e->getHitPoints();
	}
	lastHpDifference = enemiesHp - thisUnit->getHitPoints();
}

///<summary>Initializes the agent and it's starting state</summary>
///<param name="container">The state container for this type of unit</param>
///<param name="unit">The unit that will be handled by this agent</param>
Agent::Agent(StateContainer *container, BWAPI::Unit unit, float epsilon)
{
	thisUnit = unit;
	state_container = container;
	currentState = state_container->getStateByValues(isWeaponOnCooldown(), getDistanceToClosestEnemy(), numberOfEnemiesInRange(), getHealthStatus());

	BWAPI::Unitset allEnemies = BWAPI::Broodwar->enemy()->getUnits();

	int enemiesHp = 0;
	currentAction = Action::Fight;
	learningFactor = 0.05f;
	explorationEpsilon = epsilon;
	gammaFactor = 0.9f;

	srand(time(NULL));

	for (auto& e : allEnemies)
	{
		enemiesHp += e->getHitPoints();
	}
	lastHpDifference = enemiesHp - thisUnit->getHitPoints();
}

///<summary>Just creates the instance but doesn't initialize anything</summary>
Agent::Agent()
{
	thisUnit = nullptr;
	state_container = nullptr;
}

Agent& Agent::operator=(Agent &other)
{
	this->lastHpDifference = other.lastHpDifference;
	this->learningFactor = other.learningFactor;
	this->explorationEpsilon = other.explorationEpsilon;
	this->currentState = other.currentState;
	this->state_container = other.state_container;
	this->thisUnit = other.thisUnit;
	this->currentAction = other.currentAction;
	this->gammaFactor = other.gammaFactor;

	return *this;
}

///<summary>Checks if the agent is initialized</summary>
bool Agent::isInitialized()
{
	return state_container != nullptr && thisUnit != nullptr;
}

///<summary>Sets the pointer to the state container to null</summary>
Agent::~Agent()
{
	state_container = nullptr;
}

/// <summary>Returns the health, converted to HealthOfUnit enum, of this agent</summary>
/// <returns>Returns the health, converted to HealthOfUnit enum, of this agent</returns>
HealthOfUnit Agent::getHealthStatus()
{
	int hp = thisUnit->getHitPoints();
	float hpPercent = hp * 100.f / (float)thisUnit->getInitialHitPoints(); //(hp / 100.f) * thisUnit->getInitialHitPoints();
	if (hpPercent <= 25)
		return HealthOfUnit::Critical;
	else if (hpPercent <= 50)
		return HealthOfUnit::Low;
	else if (hpPercent <= 75)
		return HealthOfUnit::Medium;
	else
		return HealthOfUnit::High;
}


///<summar>Returns the closest enemy to the agent</summary>
BWAPI::Unit Agent::getClosestEnemy()
{
	return thisUnit->getClosestUnit(BWAPI::Filter::Exists && BWAPI::Filter::IsEnemy);
}

/// <summary> Returns the distance, converted to DistanceToEnemy enum, to the closest enemy</summary>
/// <returns> Returns the distance, converted to DistanceToEnemy enum, to the closest enemy</returns>
DistanceToEnemy Agent::getDistanceToClosestEnemy()
{
	BWAPI::Unit closestEnemy = getClosestEnemy();
	int dToClosestEnemy = thisUnit->getDistance(closestEnemy);

	double speedInThirtyFrames = thisUnit->getType().topSpeed() * 10; //return top speed/frame

	double distancePercent = dToClosestEnemy * 100.f / (float)speedInThirtyFrames;

	if (distancePercent <= 40)
		return DistanceToEnemy::VeryClose;
	else if (distancePercent <= 80)
		return DistanceToEnemy::Close;
	else if (distancePercent <= 120)
		return DistanceToEnemy::Normal;
	else
		return DistanceToEnemy::Far;
}

/// <summary> Whether this agent's ground weapon is on cooldown</summary>
/// <returns> Whether this agent's ground weapon is on cooldown</returns>
bool Agent::isWeaponOnCooldown()
{
	return thisUnit->getGroundWeaponCooldown() != 0;
}

/// <summary> Number of enemies in this agent's range</summary>
/// <returns> Number of enemies in this agent's range</returns>
int Agent::numberOfEnemiesInRange()
{
	return getEnemiesInRange().size();
}

///<summary>Return the reward at this moment</summary>
int Agent::currentReward()
{
	BWAPI::Unitset allEnemies = BWAPI::Broodwar->enemy()->getUnits();

	int enemiesHp = 0;

	for (auto& e : allEnemies)
	{
		enemiesHp += e->getHitPoints();
	}

	int currentHpDifference = enemiesHp - thisUnit->getHitPoints();
	int reward = lastHpDifference - currentHpDifference;
	lastHpDifference = currentHpDifference;
	return reward;
}

///<summary>Issues the attack on the most hurt enemy in the agent's range. If there is no enemy in range attack the closest one from the whole map</summary>
void Agent::Attack()
{
	if (thisUnit->canAttack())
	{
		BWAPI::Unitset enemiesInRange = getEnemiesInRange();
		BWAPI::Unit lowestHPEnemyInRange = nullptr;
		int minHp = 99999;
		for (BWAPI::Unit u : enemiesInRange)
		{
			if (u->getHitPoints() < minHp)
			{
				minHp = u->getHitPoints();
				lowestHPEnemyInRange = u;
			}
		}

		if (lowestHPEnemyInRange != nullptr)
			thisUnit->attack(lowestHPEnemyInRange);
		else
		{
			BWAPI::Unit closestEnemy = getClosestEnemy();
			if (closestEnemy)
				thisUnit->attack(closestEnemy);
		}
	}
}

///<summary>Returns all enemies in range of the agent</summary>
BWAPI::Unitset Agent::getEnemiesInRange()
{
	return thisUnit->getUnitsInWeaponRange(thisUnit->getType().groundWeapon(), BWAPI::Filter::IsEnemy && BWAPI::Filter::Exists);
}

///<summary>Makes the agent run in the best direction in order to avoid enemy units as best as possible</summary>
void Agent::Flee()
{

	//for now the distance to the enemy doesn't count. it should be fixed
	if (thisUnit->canMove())
	{
		BWAPI::Unitset allEnemies = BWAPI::Broodwar->enemy()->getUnits();

		Vector2 myPosition(thisUnit->getPosition().x, thisUnit->getPosition().y);
		Vector2 vecResult;


		for (auto& e : allEnemies)
		{
			Vector2 enemyPosition(e->getPosition().x, e->getPosition().y);
			Vector2 vecEnemyToMe = myPosition - enemyPosition;
			float distanceSq = vecEnemyToMe.lenSq();
			if (distanceSq > 0 && e->isInWeaponRange(thisUnit))//distanceSq < 400000)
			{
				vecResult = vecResult + vecEnemyToMe.normalize();
			}

		}

		if (vecResult.lenSq() == 0)
			return;

		vecResult.normalize();

		vecResult = myPosition + vecResult * 100;

		thisUnit->move(BWAPI::Position(vecResult.getX(), vecResult.getY()));
	}
}

///<summary>It should return what action to do</summary>
Action Agent::decideOnAction()
{
	int random = rand() % 100 + 1;
	if (random <= explorationEpsilon)
	{
		return (Action)(rand() % 2);
	}
	else
	{
		return currentState->getBestAction();
	}
}

///<summary>Handle this agent's update</summary>
void Agent::Update()
{
	float currentStateActionEstimatedReward = currentState->getActionValue(currentAction);
	
	auto nextState = state_container->getStateByValues(isWeaponOnCooldown(), getDistanceToClosestEnemy(), numberOfEnemiesInRange(), getHealthStatus());
	float nextStateMaxReward = nextState->getFightValue() > nextState->getRunValue() ? nextState->getFightValue() : nextState->getRunValue();

	float watkinsReward = learningFactor * (currentReward() + gammaFactor * nextStateMaxReward - currentStateActionEstimatedReward);

	currentState->influenceValue(currentAction, watkinsReward);
	
	//currentState = state_container->getStateByValues(isWeaponOnCooldown(), getDistanceToClosestEnemy(), numberOfEnemiesInRange(), getHealthStatus());
	currentState = nextState;
	currentAction = decideOnAction();
	TakeAction(currentAction);
}

///<summary>Actually take the action<summary>
void Agent::TakeAction(Action action)
{
	//std::ofstream action_log_file("actions.log");
	if (action == Action::Fight)
	{
		BWAPI::Broodwar->sendText("Attacking");
		Attack();
	}	
	else
	{
		BWAPI::Broodwar->sendText("Fleeing");
		Flee();
	}
}