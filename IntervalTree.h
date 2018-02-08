#ifndef INTERVALTREE_H
#define INTERVALTREE_H

#include <iostream>
#include <stdlib.h>
#include "IntervalNode.h"
#include <vector>

using namespace std;



class IntervalTree{

public:
	IntervalNode *root;

	IntervalNode *sentinel;
	IntervalTree();

	vector<Interval*> search(int value);

	void search(Interval *i, vector<IntervalNode*> &result);

	Interval *searchFirstOverlapping(Interval *i);

	void searchSubTree(IntervalNode *node, Interval *i, vector<IntervalNode*> &result);

	IntervalNode *findInterval(IntervalNode *tree, int destId, Interval *i);

	void searchSubTree(IntervalNode *node, int value, vector<Interval*> &result);

	void add(int destId, Interval *interval);

	void insertInterval(int destId, Interval *interval, IntervalNode *currentNode);

	void renewConstraintsAfterInsert(IntervalNode *node);

	void removeNode(IntervalNode *node);

	void renewConstraintsAfterDelete(IntervalNode *node);

	void rotateLeft(IntervalNode *node);

	void rotateRight(IntervalNode *node);

	void printTree();
};

#endif
