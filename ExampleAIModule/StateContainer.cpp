#include "StateContainer.h"
#include "fstream"
#include "algorithm"

StateContainer::StateContainer(int maxNumberOfUnits)
{
	//check if there is a save file
	std::ifstream file(filename);
	//if there is one load the q values from it
	if (file.good())
	{
		file.close();
		LoadStates();
	}
	//otherwise create a blank file
	else
	{
		file.close();
		this->maxNumberOfUnits = maxNumberOfUnits;
		for (DistanceToEnemy i = DistanceToEnemy::VeryClose; i < DistanceToEnemy::DistanceEnumSize; i = (DistanceToEnemy)(i + 1))
		{
			for (HealthOfUnit j = HealthOfUnit::Critical; j < HealthOfUnit::HPEnumSize; j = (HealthOfUnit)(j + 1))
			{
				for (int k = 0; k < maxNumberOfUnits + 1; k++)
				{
					State s1(false, i, k, j);
					State s2(true, i, k, j);
					states.push_back(s1);
					states.push_back(s2);
				}
			}
		}
		SaveStates();
	}

	//currentState = states.end();
}

std::vector<State>::iterator StateContainer::getStateByValues(bool weaponOnCooldown, DistanceToEnemy dte, int numberOfUnitsInRadius, HealthOfUnit hou)
{
	State st(weaponOnCooldown, dte, numberOfUnitsInRadius, hou);

	for (std::vector<State>::iterator iter = states.begin(); iter != states.end(); iter++)
	{
		if (st == (*iter))
			return iter;
	}

	return states.end();
}
std::vector<State>::iterator StateContainer::getStateByValues(State &s)
{
	for (std::vector<State>::iterator iter = states.begin(); iter != states.end(); iter++)
	{
		if (s == (*iter))
			return iter;
	}
}

void StateContainer::SaveStates()
{
	std::ofstream saveFile(filename);

	saveFile << maxNumberOfUnits << " ";

	for (vector<State>::const_iterator it = states.begin(); it != states.end(); it++)
	{
		saveFile << it->getRunValue() << " " << it->getFightValue() << " ";
	}

	saveFile << endl;
	saveFile.close();
}

void StateContainer::LoadStates()
{
	std::ifstream saveFile(filename);
	saveFile >> maxNumberOfUnits;

	for (DistanceToEnemy i = DistanceToEnemy::VeryClose; i < DistanceToEnemy::DistanceEnumSize; i = (DistanceToEnemy)(i + 1))
	{
		for (HealthOfUnit j = HealthOfUnit::Critical; j < HealthOfUnit::HPEnumSize; j = (HealthOfUnit)(j + 1))
		{
			for (int k = 0; k < maxNumberOfUnits + 1; k++)
			{
				State s1(false, i, k, j);
				State s2(true, i, k, j);
				float s1_run, s1_fight, s2_run, s2_fight;
				saveFile >> s1_run >> s1_fight >> s2_run >> s2_fight;

				s1.setRunValue(s1_run);
				s1.setFightValue(s1_fight);

				s2.setRunValue(s2_run);
				s2.setFightValue(s2_fight);

				states.push_back(s1);
				states.push_back(s2);
			}
		}
	}
	saveFile.close();
}

//void StateContainer::setCurrentState(bool wCd, DistanceToEnemy dte, int noUnits, HealthOfUnit hp)
//{
//	State s(wCd, dte, noUnits, hp);
//
//	for (currentState = states.begin(); currentState != states.end(); currentState++)
//	{
//		if ((*currentState) == s)
//			break;
//	}
//}

StateContainer::~StateContainer()
{
	SaveStates();
}
