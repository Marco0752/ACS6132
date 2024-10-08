/* Demo_03_Model.cpp */

#include <stdio.h>
#include <vector>
#include <boost/mpi.hpp>
#include "repast_hpc/AgentId.h"
#include "repast_hpc/RepastProcess.h"
#include "repast_hpc/Utilities.h"
#include "repast_hpc/Properties.h"
#include "repast_hpc/initialize_random.h"
#include "repast_hpc/SVDataSetBuilder.h"
#include "repast_hpc/Point.h"
#include "repast_hpc/Random.h"

#include "Model.h"


SchellingModel::SchellingModel(std::string propsFile, int argc, char** argv, boost::mpi::communicator* comm): context(comm) {
    props = new repast::Properties(propsFile, argc, argv, comm);
    stopAt = repast::strToInt(props->getProperty("stop.at"));
    countOfAgents = repast::strToInt(props->getProperty("count.of.agents"));
    boardSize = repast::strToInt(props->getProperty("board.size"));
    initializeRandom(*props, comm);

    repast::Point<double> origin(1, 1);
    repast::Point<double> extent(boardSize + 1, boardSize + 1);
    repast::GridDimensions gd(origin, extent);

    std::vector<int> processDims;
    processDims.push_back(1);
    processDims.push_back(1);

    discreteSpace = new repast::SharedDiscreteSpace<Agent, repast::StrictBorders, repast::SimpleAdder<Agent>>(
        "AgentDiscreteSpace", gd, processDims, 0, comm);
    context.addProjection(discreteSpace);

    std::cout << "RANK " << repast::RepastProcess::instance()->rank() 
              << " BOUNDS: " << discreteSpace->bounds().origin() 
              << " " << discreteSpace->bounds().extents() << std::endl;

    // TODO: read from model.props, init the shared space
}


SchellingModel::~SchellingModel(){
	delete props;
}

void SchellingModel::initAgents(){
	int rank = repast::RepastProcess::instance()->rank();
	repast::IntUniformGenerator gen = repast::Random::instance()->createUniIntGenerator(1, boardSize); //random from 1 to boardSize
	int countType0 = countOfAgents/2; //half type 0
	int countType1 = countOfAgents - countType0; // the rest type 1
	double threshold = repast::strToDouble(props->getProperty("threshold"));
	for (int i = 0; i < countOfAgents; i++){
		//random a location until an empty one is found (not the most efficient)
		int xRand, yRand;
		std::vector<Agent*> agentList;
		do {
			agentList.clear();
			xRand = gen.next(); yRand = gen.next();
			discreteSpace->getObjectsAt(repast::Point<int>(xRand, yRand), agentList);
		} while (agentList.size() != 0);


		//create agent, assign type, move the agent to the randomised location
		repast::Point<int> initialLocation(xRand, yRand);
		repast::AgentId id(i, rank, 0);
		id.currentRank(rank);
		int type;
		if (countType0 > 0) {
			type = 0;
			countType0--;
		} else {
			type = 1;
			countType1--;
		}
		Agent* agent = new Agent(id, type, threshold);
		context.addAgent(agent);
		discreteSpace->moveTo(id, initialLocation);
	}

	//TODO: init agents with a random location and a satisfied threshold (read from model.props).
}

void SchellingModel::doPerTick(){
	//calculate avg satisfaction
	double avgSatisfied = 0;
	std::vector<Agent*> agents;
	context.selectAgents(repast::SharedContext<Agent>::LOCAL, countOfAgents, agents);
	std::vector<Agent*>::iterator it = agents.begin();
	while(it != agents.end()){
		(*it)->updateStatus(&context, discreteSpace);
		avgSatisfied += (*it)->getSatisfiedStatus();
		it++;
	}
	avgSatisfied /= countOfAgents;


	//print avg satisfaction
	double currentTick = repast::RepastProcess::instance()->getScheduleRunner().currentTick();
	if(repast::RepastProcess::instance()->rank() == 0) {
		printf("Tick: %.1f\tAvg satisfied: %.2f\n", currentTick, avgSatisfied);
		if (currentTick==1 || currentTick==stopAt) //print at the beginning and the end of the simulation
			printToScreen();
	}


	//agents move to a random location if unsatisfied
	it = agents.begin();
	while(it != agents.end()){
		if (!(*it)->getSatisfiedStatus())
			(*it)->move(discreteSpace);
		it++;
	}

	//TODO: print average satisfaction of all agents
	//TODO: all disatisified agents move to a random location
}

void SchellingModel::initSchedule(repast::ScheduleRunner& runner){
	runner.scheduleEvent(1, 1, repast::Schedule::FunctorPtr(new repast::MethodFunctor<SchellingModel> (this, &SchellingModel::doPerTick)));
	runner.scheduleStop(stopAt);
}

void SchellingModel::printToScreen() {
	//print board to screen
	std::vector<Agent*> agentList;
	for (int i=0; i<=boardSize+1; i++) {
		for (int j=0; j<=boardSize+1; j++) {
			if (i==0 || i==boardSize+1)
				std::cout << "-";
			else if (j==0 || j==boardSize+1)
				std::cout << "|";
			else {
				agentList.clear();
				discreteSpace->getObjectsAt(repast::Point<int>(i, j), agentList);
				if (agentList.size() > 1) {std::cerr << "More than 1 agent per cell" << std::endl;}
				if (agentList.size() == 0)
					std::cout << " ";
				else if ((agentList.front())->getType() == 0)
					std::cout << "X";
				else if ((agentList.front())->getType() == 1)
					std::cout << ".";
			}
		}
		std::cout << std::endl;
	}
}
