#include <iostream>
#include <stdlib.h>

#include "TemporalEdge.h"

TemporalEdge::TemporalEdge(int isDestId, int isStartTime){
	destId = isDestId;
	interval = new Interval(isStartTime);
}

bool TemporalEdge::isValid(int start, int end){
	Interval *temp1 = new Interval(start,end);
	Interval *temp2 = interval->intersection(temp1);
	bool answer = false;
	if (temp2) answer = true;
	free(temp1);
	free(temp2);
	return answer;
}

int TemporalEdge::getEndTime(){
	return interval->getEndTime();
}

int TemporalEdge::getStartTime(){
	return interval->getStartTime();
}

int TemporalEdge::getDestId(){
	return destId;
}

void TemporalEdge::setEndTime(int isEndTime){
	interval->setEndTime(isEndTime);
}

Interval* TemporalEdge::getInterval(){
	return interval;
}


bool TemporalEdge::operator <( const TemporalEdge &rhs ) const{
	if ( destId < rhs.destId ) return true;
	if ((destId == rhs.destId)&&(interval->getStartTime() < rhs.interval->getStartTime())) return true;
	if ((destId == rhs.destId)&&(interval->getStartTime() == rhs.interval->getStartTime())&&(interval->getEndTime() < rhs.interval->getEndTime())) return true;
	return false;
}
