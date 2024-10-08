#ifndef SCHELLING_AGENT
#define SCHELLING_AGENT

#include "repast_hpc/AgentId.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/SharedDiscreteSpace.h"


class Agent{
	
private:
	repast::AgentId agentId;
	int agentType;
	bool isSatisfied;
	double threshold;
	
public:
	Agent(repast::AgentId id, int agentType, double threshold); //for init
	~Agent();
	
	/* Required Getters */
	virtual repast::AgentId& getId() { return agentId; }
	virtual const repast::AgentId& getId() const { return agentId; }
	
	/* Getters specific to this kind of Agent */
	int getType() { return agentType; }
	bool getSatisfiedStatus() { return isSatisfied; }
	
	/* Actions */
	void updateStatus(repast::SharedContext<Agent>* context,
			  repast::SharedDiscreteSpace<Agent, repast::StrictBorders, repast::SimpleAdder<Agent> >* space);
	void move(repast::SharedDiscreteSpace<Agent, repast::StrictBorders, repast::SimpleAdder<Agent> >* space);	
};

#endif