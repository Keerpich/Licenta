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
	FightEnemy = 0, 
	FightAlly,
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

	//expected values
	float runStateValue;
	float fightEnemyStateValue;
	float fightAllyStateValue;

public:
	void setRunValue(float value);
	void setFightEnemyValue(float value);
	void setFightAllyValue(float value);
	float getRunValue() const;
	float getFightEnemyValue() const;
	float getFightAllyValue() const;
	float getActionValue(Action action) const;

	void influenceValue(Action action, float value);

	Action getBestAction() const;

	State(bool, DistanceToEnemy, int, HealthOfUnit);
	~State();

	bool operator==(State& s);
	bool operator==(const State& s);
};

