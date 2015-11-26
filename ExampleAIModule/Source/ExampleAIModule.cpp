#include "ExampleAIModule.h"
#include <iostream>
#include "../Agent.h"
#include <fstream>

using namespace BWAPI;
using namespace Filter;

StateContainer state_container(32);
vector<Agent> vultureAgents;
int ExampleAIModule::roundCount = 0;

void ExampleAIModule::onStart()
{
	std::ifstream round_file("p:\\Licenta\\VultureLearning\\rnd_count.txt");
	if (round_file)
	{
		round_file >> roundCount;
		round_file.close();
	}
	roundCount++;
	std::ofstream rnd_file("p:\\Licenta\\VultureLearning\\rnd_count.txt");
	rnd_file << roundCount;

	BWAPI::Broodwar->enableFlag(BWAPI::Flag::CompleteMapInformation);
	BWAPI::Broodwar->setLocalSpeed(0);

	Broodwar->sendText("Round Count: %i", roundCount);

	Unitset units = Broodwar->self()->getUnits();
	std::vector<Unit> vultureUnits;
	for (auto& u : units)
	{
		int id = u->getID();
		Unit vultureUnit = Broodwar->getUnit(id);
		if (vultureUnit->getType() == UnitTypes::Terran_Vulture)
		{
			vultureUnits.push_back(vultureUnit);
		}
	}

	for (int i = 0; i < vultureUnits.size(); i++)
	{
		Agent vultureAgent;
		if (roundCount > 1000)
			vultureAgent = Agent(&state_container, vultureUnits[i], 0.f);
		else
			vultureAgent = Agent(&state_container, vultureUnits[i], 100 - (roundCount / 10.f));

		vultureAgents.push_back(vultureAgent);
	}
  // Set the command optimization level so that common commands can be grouped
  // and reduce the bot's APM (Actions Per Minute).
  

	Broodwar->setCommandOptimizationLevel(2);

  // Check if this is a replay
  if ( Broodwar->isReplay() )
  {

    // Announce the players in the replay
    Broodwar << "The following players are in this replay:" << std::endl;
    
    // Iterate all the players in the game using a std:: iterator
    Playerset players = Broodwar->getPlayers();
    for(auto p : players)
    {
      // Only print the player if they are not an observer
      if ( !p->isObserver() )
        Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;
    }

  }
  else // if this is not a replay
  {

	  //Should handle the graph load here

    // Retrieve you and your enemy's races. enemy() will just return the first enemy.
    // If you wish to deal with multiple enemies then you must use enemies().
    if ( Broodwar->enemy() ) // First make sure there is an enemy
      Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;
  }

}

void ExampleAIModule::onEnd(bool isWinner)
{

	Broodwar->sendText(isWinner ? "You have won" : "You have lost");
	//Save the Q-Learning graph
	state_container.SaveStates();
	//Restart the map
	Broodwar->restartGame();
}

void ExampleAIModule::onFrame()
{
	//static int lastCommand = 10;
	//lastCommand++;
	//if (lastCommand > 10)
	//{
	//	lastCommand = 0;
	//	if (vultureAgent.isInitialized())
	//		vultureAgent.Update();
	//}

	for (auto& a : vultureAgents)
	{
		if (a.isInitialized())
			a.Update();
	}
}

void ExampleAIModule::onSendText(std::string text)
{

  // Send the text to the game if it is not being processed.
  Broodwar->sendText("%s", text.c_str());


  // Make sure to use %s and pass the text as a parameter,
  // otherwise you may run into problems when you use the %(percent) character!

}

void ExampleAIModule::onReceiveText(BWAPI::Player player, std::string text)
{
  // Parse the received text
  Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void ExampleAIModule::onPlayerLeft(BWAPI::Player player)
{
  // Interact verbally with the other players in the game by
  // announcing that the other player has left.
  Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void ExampleAIModule::onNukeDetect(BWAPI::Position target)
{

  // Check if the target is a valid position
  if ( target )
  {
    // if so, print the location of the nuclear strike target
    Broodwar << "Nuclear Launch Detected at " << target << std::endl;
  }
  else 
  {
    // Otherwise, ask other players where the nuke is!
    Broodwar->sendText("Where's the nuke?");
  }

  // You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void ExampleAIModule::onUnitDiscover(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitEvade(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitShow(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitHide(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitCreate(BWAPI::Unit unit)
{
  if ( Broodwar->isReplay() )
  {
    // if we are in a replay, then we will print out the build order of the structures
    if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() )
    {
      int seconds = Broodwar->getFrameCount()/24;
      int minutes = seconds/60;
      seconds %= 60;
      Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
    }
  }
}

void ExampleAIModule::onUnitDestroy(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitMorph(BWAPI::Unit unit)
{
  if ( Broodwar->isReplay() )
  {
    // if we are in a replay, then we will print out the build order of the structures
    if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() )
    {
      int seconds = Broodwar->getFrameCount()/24;
      int minutes = seconds/60;
      seconds %= 60;
      Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
    }
  }
}

void ExampleAIModule::onUnitRenegade(BWAPI::Unit unit)
{
}

void ExampleAIModule::onSaveGame(std::string gameName)
{
  Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void ExampleAIModule::onUnitComplete(BWAPI::Unit unit)
{
}