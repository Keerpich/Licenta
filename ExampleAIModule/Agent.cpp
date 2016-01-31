#include "Agent.h"
#include <ctime>
#include <fstream>

#define DEBUG 0

///<summary>Initializes the agent and it's starting state</summary>
///<param name="container">The state container for this type of unit</param>
///<param name="unit">The unit that will be handled by this agent</param>
Agent::Agent(StateContainer *container, BWAPI::Unit unit)
{
	thisUnit = unit;
	state_container = container;
	currentState = state_container->getStateByValues(isWeaponOnCooldown(), getDistanceToClosestEnemy(), numberOfEnemiesInRange(), getHealthStatus(), canUseMineNow());

	BWAPI::Unitset allEnemies = BWAPI::Broodwar->enemy()->getUnits();

	framesPassed = 0;
	framesNeeded = 0;
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
	currentState = state_container->getStateByValues(isWeaponOnCooldown(), getDistanceToClosestEnemy(), numberOfEnemiesInRange(), getHealthStatus(), canUseMineNow());

	BWAPI::Unitset allEnemies = BWAPI::Broodwar->enemy()->getUnits();

	int enemiesHp = 0;
	currentAction = Action::Fight;
	learningFactor = 0.05f;
	explorationEpsilon = epsilon;
	waitingForMine = false;
	gammaFactor = 0.9f;
	framesPassed = 0;
	framesNeeded = 0;

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
	this->framesNeeded = other.framesNeeded;
	this->framesPassed = other.framesPassed;
	this->waitingForMine = other.waitingForMine;
	this->allMines = other.allMines;

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

	double speedInThirtyFrames = thisUnit->getType().topSpeed() * 18; //return top speed/frame

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
		{
			thisUnit->attack(lowestHPEnemyInRange);
			//framesNeeded = (thisUnit->getDistance(lowestHPEnemyInRange) - thisUnit->getType().groundWeapon().maxRange()) / thisUnit->getType().topSpeed();
			framesNeeded = 15;
			framesPassed = 0;

			//BWAPI::Broodwar->sendText("frames needed: %i", framesNeeded);
		}
		else
		{
			BWAPI::Unit closestEnemy = getClosestEnemy();
			if (closestEnemy)
			{
				thisUnit->attack(closestEnemy);
				framesNeeded = (thisUnit->getDistance(closestEnemy) - thisUnit->getType().groundWeapon().maxRange()) / (thisUnit->getType().topSpeed() + closestEnemy->getType().topSpeed()) - 3;
				framesNeeded = framesNeeded > 15 ? framesNeeded : 15;
				framesPassed = 0;


				//BWAPI::Broodwar->sendText("frames needed: %i", framesNeeded);
			}
		}
	}
}

///<summary>Returns all enemies in range of the agent</summary>
BWAPI::Unitset Agent::getEnemiesInRange()
{
	return thisUnit->getUnitsInWeaponRange(thisUnit->getType().groundWeapon(), BWAPI::Filter::IsEnemy && BWAPI::Filter::Exists);
}


void Agent::Deploy()
{
	framesNeeded = 0;
	framesPassed = 0;

#if DEBUG
	std::ofstream debug_file("p:\\Licenta\\VultureLearning\\debug.txt", std::ios::app);
	debug_file << "Deploy - Beginning" << endl;
#endif

	if (thisUnit->canUseTech(BWAPI::TechTypes::Spider_Mines, thisUnit->getPosition()))
	{
#if DEBUG
		debug_file << "Deploy - It can use tech" << endl;
#endif
		if (thisUnit->useTech(BWAPI::TechTypes::Spider_Mines, thisUnit->getPosition()))
		{
#if DEBUG
			debug_file << "Deploy - It deployed the mine" << endl;
#endif
			Mine thisMine;
#if DEBUG
			debug_file << "Deploy - After mine constructor" << endl;
#endif
			thisMine.deployedInState = currentState;
			thisMine.mineId = INT_MIN;
#if DEBUG
			debug_file << "Deploy - Initialized mine" << endl;
#endif
			waitingForMine = true;
#if DEBUG
			debug_file << "Deploy - waiting for mine is true" << endl;
#endif
			allMines.push_back(thisMine);
#if DEBUG
			debug_file << "Deploy - Added mine to vector" << endl;
			debug_file.close();
#endif
			framesNeeded = 20;
		}
	}
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
			if (distanceSq > 0 && thisUnit->isInWeaponRange(e))//distanceSq < 400000)
			{
				vecResult = vecResult + vecEnemyToMe.normalize();
			}

		}

		framesNeeded = 0;
		framesPassed = 0;

		if (vecResult.lenSq() == 0)
			return;

		vecResult.normalize();

		vecResult = myPosition + vecResult * 300;

		thisUnit->move(BWAPI::Position(vecResult.getX(), vecResult.getY()));

		framesNeeded = (int)(300 / thisUnit->getType().topSpeed()) + 1;
		framesPassed = 0;
	}
}

///<summary>It should return what action to do</summary>
Action Agent::decideOnAction()
{
	int random = rand() % 100 + 1;
	if (random <= explorationEpsilon)
	{
		return (Action)(rand() % 3);
	}
	else
	{
		return currentState->getBestAction();
	}
}

void Agent::GetAllSpiderMines(BWAPI::Unitset &allSpiderMines)
{
#if DEBUG
	std::ofstream debug_file("p:\\Licenta\\VultureLearning\\debug.txt", std::ios::app);
	debug_file << "GetAllSpiderMines - Beginning" << endl;
#endif
	BWAPI::Unitset allMyUnits = thisUnit->getPlayer()->getUnits();

#if DEBUG
	debug_file << "GetAllSpiderMines - Got the player's units (count: " << allMyUnits.size() << ")" << endl;
#endif

	for (BWAPI::Unitset::iterator it = allMyUnits.begin(); it != allMyUnits.end(); it++)
	{
#if DEBUG
		debug_file << "GetAllSpiderMines - beginning of the for loop" << endl;
#endif
		if ((*it)->getType() == BWAPI::UnitTypes::Terran_Vulture_Spider_Mine && (*it)->exists())
		{
#if DEBUG
			debug_file << "GetAllSpiderMines - this unit is a spider mine" << endl;
#endif
			allSpiderMines.insert((*it));
#if DEBUG
			debug_file << "GetAllSpiderMines - inserted the spider mine" << endl;
#endif
		}
	}

#if DEBUG
	debug_file << "GetAllSpiderMines - we got " << allSpiderMines.size() << " spider mines" << endl;
	debug_file.close();
#endif
}

bool Agent::hasAnyMinesNotAdded()
{
#if DEBUG
	std::ofstream debug_file("p:\\Licenta\\VultureLearning\\debug.txt", std::ios::app);
	debug_file << "hasAnyMinesNotAdded - Beginning" << endl;
#endif

	if (waitingForMine)
	{
#if DEBUG
		debug_file << "hasAnyMinesNotAdded - waitingForMine is true" << endl;
#endif

		for (int i = 0; i < allMines.size(); i++)
		{

#if DEBUG
			debug_file << "hasAnyMinesNotAdded - Loop number: " << i << endl;
			debug_file << "hasAnyMinesNotAdded - mineId: " << allMines[i].mineId << endl;
#endif

			if (allMines[i].mineId == INT_MIN)
			{

#if DEBUG
				debug_file << "hasAnyMinesNotAdded - mineId is INT_MIN" << endl;
#endif

				BWAPI::Unitset allSpiderMines;
#if DEBUG
				debug_file << "hasAnyMinesNotAdded - created allSpiderMines" << endl;
#endif
				GetAllSpiderMines(allSpiderMines);

#if DEBUG
				debug_file << "hasAnyMinesNotAdded - Got all spider mines" << endl;
#endif

				for (auto &unit : allSpiderMines)
				{

#if DEBUG
					debug_file << "hasAnyMinesNotAdded - In spider mines auto loop" << endl;
#endif
					if (unit->getType() == BWAPI::UnitTypes::Terran_Vulture_Spider_Mine)
					{
#if DEBUG
						debug_file << "hasAnyMinesNotAdded - This unit is a spider mine" << endl;
#endif
						bool isRecordedMine = false;
						for (int j = 0; j < allMines.size(); j++)
						{
							isRecordedMine = (allMines[j].mineId == unit->getID());
						}


#if DEBUG
						debug_file << "hasAnyMinesNotAdded - after the isRecordedMine loop (" << isRecordedMine << ")" << endl;
#endif

						if (!isRecordedMine)
						{
#if DEBUG
							debug_file << "hasAnyMinesNotAdded - it's not a recorded mine" << endl;
#endif
							waitingForMine = false;
							allMines[i].mineId = unit->getID();

#if DEBUG
							debug_file << "hasAnyMinesNotAdded - mine's id is now :" << allMines[i].mineId << endl;
							debug_file.close();
#endif
							return false;
						}
					}
				}


			}
		}

#if DEBUG
		debug_file.close();
#endif
		return true;
	}
	else
	{
#if DEBUG
		debug_file.close();
#endif
		return false;
	}
}

void Agent::influenceStateByMineExplosion()
{
#if DEBUG
	if (allMines.size() == 0)
		return;
#endif

#if DEBUG
	std::ofstream debug_file("p:\\Licenta\\VultureLearning\\debug.txt", std::ios::app);
	debug_file << "influenceStateByMineExplosion - Beginning" << endl;
#endif

	//check if there are any mines that exploded
	vector<Mine>::iterator explodedMine = allMines.end();

#if DEBUG
	debug_file << "influenceStateByMineExplosion - initalized explodedMine" << endl;
#endif
	
	for (explodedMine = allMines.begin(); explodedMine < allMines.end(); explodedMine++)
	{
#if DEBUG
		debug_file << "influenceStateByMineExplosion - beginning of for loop" << endl;
		debug_file << "influenceStateByMineExplosion - looking for mine with it" << explodedMine->mineId << endl;
#endif
		BWAPI::Unit currentMine = BWAPI::Broodwar->getUnit(explodedMine->mineId);

#if DEBUG
		debug_file << "influenceStateByMineExplosion - got the unit and it is null(" << (currentMine==nullptr ? "true" : "false") << ")" << endl;
#endif

		if (currentMine!=nullptr && !BWAPI::Broodwar->getUnit(explodedMine->mineId)->exists())
		{
#if DEBUG
			debug_file << "influenceStateByMineExplosion - found exploded mine with id " << explodedMine->mineId << endl;
#endif
			break;
		}
	}

#if DEBUG
	debug_file << "influenceStateByMineExplosion - Checked if mine exploded (" << (explodedMine != allMines.end() ? "true" : "false") << ")" << endl;
#endif

	//influence the state
	if (explodedMine != allMines.end())
	{
#if DEBUG
		debug_file << "influenceStateByMineExplosion - a mine has exploded" << endl;
#endif
		float currentStateActionEstimatedReward = currentState->getActionValue(currentAction);
#if DEBUG
		debug_file << "influenceStateByMineExplosion - calculated currentStateActionEstimatedReward" << endl;
#endif
		auto nextState = state_container->getStateByValues(isWeaponOnCooldown(), getDistanceToClosestEnemy(), numberOfEnemiesInRange(), getHealthStatus(), canUseMineNow());
#if DEBUG
		debug_file << "influenceStateByMineExplosion - got nextState" << endl;
#endif
		float nextStateMaxReward = nextState->getFightValue() > nextState->getRunValue() ? nextState->getFightValue() : nextState->getRunValue();
#if DEBUG
		debug_file << "influenceStateByMineExplosion - got nextStateMaxReward" << endl;
#endif
		float watkinsReward = learningFactor * (currentReward() + gammaFactor * nextStateMaxReward - currentStateActionEstimatedReward);
#if DEBUG
		debug_file << "influenceStateByMineExplosion - calculated watkinsReward" << endl;
#endif
		explodedMine->deployedInState->influenceValue(Action::DeployMine, watkinsReward);
#if DEBUG
		debug_file << "influenceStateByMineExplosion - influenced value" << endl;
#endif
		//remove the mine from the vector
		allMines.erase(explodedMine);
#if DEBUG
		debug_file << "influenceStateByMineExplosion - erased mine from vector" << endl;
#endif
	}
#if DEBUG
	debug_file << "influenceStateByMineExplosion - Ending" << endl;
	debug_file.close();
#endif

}

///<summary>Handle this agent's update</summary>
void Agent::Update()
{
#if DEBUG
	std::ofstream debug_file("p:\\Licenta\\VultureLearning\\debug.txt", std::ios::app);
	debug_file << "Update - Beginning" << endl;
	debug_file.close();
#endif


	if (hasAnyMinesNotAdded())
		return;

	influenceStateByMineExplosion();


	if (framesPassed++ < framesNeeded)
	{
		return;
	}
	float currentStateActionEstimatedReward = currentState->getActionValue(currentAction);
	
	auto nextState = state_container->getStateByValues(isWeaponOnCooldown(), getDistanceToClosestEnemy(), numberOfEnemiesInRange(), getHealthStatus(), canUseMineNow());
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
#if DEBUG
	std::ofstream debug_file("p:\\Licenta\\VultureLearning\\debug.txt", std::ios::app);
#endif
	if (action == Action::Fight)
	{
#if DEBUG
		debug_file << "TakeAction - Attacking" << endl;
		debug_file.close();
#endif
		BWAPI::Broodwar->sendText("Attacking");
		Attack();
	}	
	else if (action == Action::DeployMine)
	{
#if DEBUG
		debug_file << "TakeAction - Deploying Mine" << endl;
		debug_file.close();
#endif
		BWAPI::Broodwar->sendText("Deploying Mine");
		Deploy();
	}
	else
	{
#if DEBUG
		debug_file << "TakeAction - Fleeing" << endl;
		debug_file.close();
#endif
		BWAPI::Broodwar->sendText("Fleeing");
		Flee();
	}
}

bool Agent::canUseMineNow()
{
	return thisUnit->canUseTech(BWAPI::TechTypes::Spider_Mines, thisUnit->getPosition());
}