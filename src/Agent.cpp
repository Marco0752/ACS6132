#include "Agent.h"
#include "repast_hpc/Moore2DGridQuery.h"
#include "repast_hpc/Point.h"
#include <stdio.h>

Agent::Agent(repast::AgentId id, int type, double threshold): agentId(id), agentType(type), threshold(threshold) {}

Agent::~Agent() {}


void Agent::updateStatus(repast::SharedContext<Agent>* context,
		repast::SharedDiscreteSpace<Agent, repast::StrictBorders, repast::SimpleAdder<Agent> >* space)
{
	std::vector<Agent*> neighbourAgents; //to store the neighbours
	
	//get agent location from the space
	std::vector<int> agentLoc;
	space->getLocation(agentId, agentLoc);


	//get neighbours from the space (8 Moore neighbours)
	repast::Point<int> center(agentLoc);
	repast::Moore2DGridQuery<Agent> moore2DQuery(space);
	moore2DQuery.query(center, 1, false, neighbourAgents);
	
	//count similar agents in neighbours
	int similarCount = 0;
	std::vector<Agent*>::iterator tempAgent = neighbourAgents.begin();
	while(tempAgent != neighbourAgents.end()) {
		if ((*tempAgent)->getType() == agentType) {
			similarCount++;
		}


		tempAgent++;
	}


	//divide the count by the number of neighbours (not always 8)
	isSatisfied = ((double)similarCount/neighbourAgents.size() >= threshold);

	//TODO: find agents in the neighbourhood, count their type, compare with threshold, update status (satisfied or not)
}

void Agent::move(repast::SharedDiscreteSpace<Agent, repast::StrictBorders, repast::SimpleAdder<Agent> >* space)
{
	//get agent location from the space
	std::vector<int> agentLoc;
	space->getLocation(agentId, agentLoc);
	
	//random a location until an empty one is found
	int xMax = (int)space->bounds().extents(0) - 1;
	repast::IntUniformGenerator gen = repast::Random::instance()->createUniIntGenerator(1, xMax);
	std::vector<int> agentNewLoc;
	do {
		agentNewLoc.clear();
		
		int xRand, yRand;
		std::vector<Agent*> agentList;
		do {
			agentList.clear();
			xRand = gen.next();
			yRand = gen.next();
			space->getObjectsAt(repast::Point<int>(xRand, yRand), agentList);
		} while (agentList.size() != 0);


		agentNewLoc.push_back(xRand);
		agentNewLoc.push_back(yRand);
	} while(!space->bounds().contains(agentNewLoc));
	
	//move the agent
	space->moveTo(agentId,agentNewLoc);

	//TODO: find a random empty cell and move to it
}

