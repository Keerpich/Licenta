#pragma once

enum DistanceToEnemy
{
	VeryClose = 0, // <= 25% din distanta maxima de parcurs in 0.5 sec
	Close, // > 25%, <=75%
	Normal, // > 75%, <=120%
	Far, // >120%
	DistanceEnumSize
};

enum HealthOfUnit
{
	Critical = 0, //<=25%
	Low, // 25 < x <= 50%
	Medium, //50% < x <= 75%
	High, // 75 < x <=100%
	HPEnumSize
};


enum Action 
{	
	Fight = 0,
	DeployMine,
	Run 
};

class State
{
private:
	//state characteristics
	bool isWeaponOnCooldown;
	DistanceToEnemy distanceToClosestEnemy;
	int enemiesInRange; //number of enemy units in range of weapon
	HealthOfUnit agentHealth;
	bool canUseMine;

	//expected values
	float runStateValue;
	float fightStateValue;
	float deployMineStateValue;

public:
	void setRunValue(float value);
	void setFightValue(float value);
	void setDeployMineValue(float value);
	float getRunValue() const;
	float getFightValue() const;
	float getDeployMineValue() const;
	float getActionValue(Action action) const;

	void influenceValue(Action action, float value);

	Action getBestAction() const;

	State(bool, DistanceToEnemy, int, HealthOfUnit, bool);
	~State();

	bool operator==(State& s);
	bool operator==(const State& s);
};

