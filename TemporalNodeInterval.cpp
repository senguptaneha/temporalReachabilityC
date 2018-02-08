#include <vector>
#include <iostream>
#include <stdlib.h>

#include "TemporalNode.h"
#include "IntervalTree.h"


TemporalNode::TemporalNode(int isNodeId){
	nodeId = isNodeId;
	t = new IntervalTree();
}

void TemporalNode::addEdge(int destId, int startTime, bool direction){
	if (direction){
		fwdEdges.push_back(new TemporalEdge(destId, startTime));
		Interval *i = new Interval(startTime);
		t->add(destId, i);
	}
	//else do nothing (IntervalTree approach does not require reverse edges
}

int TemporalNode::removeEdge(int destId, int endTime, bool direction){
	int startTime = -1;
	if (direction){
		for (int i = 0; i < fwdEdges.size(); i++){
			if ((fwdEdges[i]->getDestId() == destId) && (fwdEdges[i]->getEndTime() == -1)){
				fwdEdges[i]->setEndTime(endTime);
				startTime = fwdEdges[i]->getStartTime();
				break;
			}
		}
		if (startTime == -1) return -1;
		Interval *i = new Interval(startTime, -1);
		IntervalNode *n = t->findInterval(t->root, destId, i);
		t->removeNode(n);
		free(i);
		i = new Interval(startTime, endTime);
		t->add(destId, i);
	}
	return startTime;
}

int TemporalNode::getNumEdges(bool direction){
	return fwdEdges.size();
}

TemporalEdge* TemporalNode::getEdgeAt(int i, bool direction){
	return fwdEdges[i];
}

vector<IntervalNode*> TemporalNode::getOverlappingEdges(Interval *x, bool direction){
	vector<IntervalNode*> foundIntervals;
	t->search(x, foundIntervals);
	return foundIntervals;
}
