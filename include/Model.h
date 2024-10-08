#ifndef MODEL
#define MODEL

#include <boost/mpi.hpp>
#include "repast_hpc/Schedule.h"
#include "repast_hpc/Properties.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/SharedDiscreteSpace.h"
#include "repast_hpc/GridComponents.h"

#include "Agent.h"

class SchellingModel{
private:
	int stopAt;
	int countOfAgents;
	int boardSize;
	void printToScreen();
	repast::Properties* props;
	repast::SharedContext<Agent> context;
	repast::SharedDiscreteSpace<Agent, repast::StrictBorders, repast::SimpleAdder<Agent> >* discreteSpace;
	
public:
	SchellingModel(std::string propsFile, int argc, char** argv, boost::mpi::communicator* comm);
	~SchellingModel();
	void initAgents();
	void initSchedule(repast::ScheduleRunner& runner);
	void doPerTick();
};

#endif
