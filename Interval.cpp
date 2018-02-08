#include <iostream>
#include <stdlib.h>
#include "Interval.h"

Interval::Interval(){
	startTime = -1;
	endTime = -1;
}

Interval::Interval(int isStartTime){
	startTime = isStartTime;
	endTime = -1;
}

Interval::Interval(int isStartTime, int isEndTime){
	startTime = isStartTime;
	endTime = isEndTime;
}


Interval* Interval::intersection(Interval *other){
	if (other == NULL) return NULL;
	if ((endTime == -1) && (other->endTime == -1))
		return new Interval(max(startTime, other->startTime), -1);
	if ((endTime == -1) && (startTime <= other->endTime))
		return new Interval(max(startTime, other->startTime), other->endTime);
	if ((other->endTime == -1) && (other->startTime <= endTime))
		return new Interval(max(startTime, other->startTime), endTime);
	if ((endTime < other->startTime) || (other->endTime < startTime)) return NULL;
	return new Interval(max(startTime, other->startTime), min(endTime, other->endTime));
}

bool Interval::intersectionR(Interval *other, Interval *result){
	if (other == NULL) return false;
	if ((endTime == -1) && (other->endTime == -1)){
		result->setStartTime(max(startTime, other->startTime));
		result->setEndTime(-1);
		return true;
	}
	if ((endTime == -1) && (startTime <= other->endTime)){
		result->setStartTime(max(startTime, other->startTime));
		result->setEndTime(other->endTime);
		return true;
	}
	if ((other->endTime == -1) && (other->startTime <= endTime)){
		result->setStartTime(max(startTime, other->startTime));
		result->setEndTime(endTime);
		return true;
	}
	if ((endTime < other->startTime) || (other->endTime < startTime)) return false;
	result->setStartTime(max(startTime, other->startTime));
	result->setEndTime(min(endTime, other->endTime));
	return true;

}

bool Interval::doesOverlap(Interval *other){
	Interval *tmp = intersection(other);
	bool answer = false;
	if (tmp) answer = true;
	free(tmp);
	return answer;
}

int Interval::compareTo(Interval *other){
	if (startTime < other->startTime) return -1;
	if (startTime > other->startTime) return 1;
	if ((endTime == -1) && (other->endTime >= 0)) return -1;
	if ((other->endTime == -1 ) && (endTime >= 0)) return 1;
	if (endTime > other->endTime) return -1;
	if (other->endTime > endTime) return 1;
	return 0;
}

bool Interval::contains(int t){
	if (t < startTime) return false;
	if ((endTime == -1) && (t >= startTime)) return true;
	if (t <= endTime) return true;
	return false;
}

bool Interval::isSubInterval(Interval *other){
	if (other == NULL) return false;
	bool answer = false;
	if ((endTime == -1) && (other->startTime >= startTime)) answer = true;
	else if ((endTime != -1) && (other->endTime != -1) &&
		(other->endTime <= endTime) && (other->startTime >= startTime))
	answer = true;
	return answer;
}

bool Interval::isSubIntervalT(int sTime, int eTime){
    bool answer = false;
    if ((endTime == -1) && (sTime >= startTime)) answer = true;
	else if ((endTime != -1) && (eTime != -1) &&
		(eTime <= endTime) && (sTime >= startTime))
	answer = true;
	return answer;
}

void Interval::setEndTime(int isEndTime){
	endTime = isEndTime;
}

void Interval::setStartTime(int isStartTime){
	startTime = isStartTime;
}

int Interval::getStartTime(){
	return startTime;
}

int Interval::getEndTime(){
	return endTime;
}

int Interval::min(int x1, int x2){
	return ((x1 < x2)? x1 : x2);
}

int Interval::max(int x1, int x2){
	return ((x1 > x2)? x1 : x2);
}

