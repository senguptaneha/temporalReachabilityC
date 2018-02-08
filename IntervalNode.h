#ifndef INTERVALNODE_H
#define INTERVALNODE_H

#include <iostream>
#include <stdlib.h>
#include "Interval.h"

class IntervalNode{

public:
	//helper function
	static int compareTimes(int t1, int t2);

	IntervalNode *left;
	IntervalNode *right;
	IntervalNode *parent;

	int maxEndTime;

	Interval *interval;
	int destId;

	int color;

	IntervalNode(); //to be used only for creating sentinel
	IntervalNode(int isDestId, IntervalNode *sentinel);


	IntervalNode(int isDestId, Interval *isInterval, IntervalNode *sentinel);

	bool isRoot(IntervalNode *sentinel);

	bool isLeaf(IntervalNode *sentinel);

	/*0 if parent is null, -1 if I am left child, 1 otherwise*/
	int parentDirection(IntervalNode *sentinel);

	IntervalNode *successor(IntervalNode *sentinel);

	int compareTo(IntervalNode *other, IntervalNode *sentinel);

	void recalculateMaxEnd(IntervalNode * sentinel);

	IntervalNode *grandParent(IntervalNode *sentinel);

	IntervalNode *uncle(IntervalNode *sentinel);

	IntervalNode *sibling(IntervalNode *sentinel);

	void printInterval();
};

#endif
