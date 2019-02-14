#ifndef TEMPORALEDGE_H
#define TEMPORALEDGE_H
#include "Interval.h"

class TemporalEdge{

public:
	TemporalEdge(int isDestId, int isStartTime);
	bool isValid(int start, int end);
	int getEndTime();
	int getStartTime();
	int getDestId();
	void setEndTime(int isEndTime);
	Interval *getInterval();
	bool operator <( const TemporalEdge &rhs ) const;


private:
	int destId;
	Interval *interval;

};

#endif
