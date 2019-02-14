#ifndef STOP_H
#define STOP_H

#include "Interval.h"

class Stop{
public:
	Stop();
	Stop(int isNodeId, Interval isInterval);
	void reset();
	int getNodeId();
	void setNodeId(int isNodeId);
	void setStartTime(int isStartTime);
	void setEndTime(int isEndTime);
	int getStartTime();
	int getEndTime();
	int intersect(Stop other);
	int numWalks;
	int walkLength;

private:
	int nodeId;
	Interval interval;
};

#endif
