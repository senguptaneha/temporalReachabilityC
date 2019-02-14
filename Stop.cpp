#include <stdlib.h>

#include "Stop.h"

Stop::Stop(){
	nodeId = -1;
	numWalks = 0;
	walkLength = 0;
};

Stop::Stop(int isNodeId, Interval isInterval){
	nodeId = isNodeId;
	interval = isInterval;
	numWalks = 0;
	walkLength = 0;
}

void Stop::reset(){
	nodeId = -1;
	numWalks = 0;
	walkLength = 0;
}

int Stop::getNodeId(){
	return nodeId;
}

void Stop::setNodeId(int isNodeId){
	nodeId = isNodeId;
}

void Stop::setStartTime(int isStartTime){
	interval.setStartTime(isStartTime);
}

void Stop::setEndTime(int isEndTime){
	interval.setEndTime(isEndTime);
}

int Stop::getStartTime(){
	return interval.getStartTime();
}

int Stop::getEndTime(){
	return interval.getEndTime();
}

int Stop::intersect(Stop other){
	if (!(nodeId == other.nodeId)) return 0;
	if (!interval.doesOverlap(&(other.interval))) return 0;
	int s = interval.getStartTime(), e = interval.getEndTime();
	if (s < other.getStartTime()) s = other.getStartTime();
	if (other.getEndTime() != -1){
		if (e == -1) e = other.getEndTime();
		else if (e > other.getEndTime()) e = other.getEndTime();
	}
	return (e - s + 1);
}


