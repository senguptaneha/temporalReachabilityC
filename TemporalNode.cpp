#include <vector>
#include <iostream>
#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "TemporalNode.h"

TemporalNode::TemporalNode(int isNodeId){
	nodeId = isNodeId;
	numFwdEdges = 0;
	numBwdEdges = 0;
}

void TemporalNode::addEdge(int destId, int startTime, bool direction){
	if (direction){
		fwdEdges.push_back(new TemporalEdge(destId, startTime));
		numFwdEdges += 1;
	}
	else{
		bwdEdges.push_back(new TemporalEdge(destId, startTime));
		numBwdEdges += 1;
	}
}

int TemporalNode::removeEdge(int destId, int endTime, bool direction){
	int startTime = -1;
	if (direction){
		for (int i = 0; i < fwdEdges.size(); i++){
			if ((fwdEdges[i]->getDestId() == destId) && (fwdEdges[i]->getEndTime() == -1)){
				fwdEdges[i]->setEndTime(endTime);
				startTime = fwdEdges[i]->getStartTime();
			}
		}
	}
	else{
		for (int i = 0; i < bwdEdges.size(); i++){
			if ((bwdEdges[i]->getDestId() == destId) && (bwdEdges[i]->getEndTime() == -1)){
				bwdEdges[i]->setEndTime(endTime);
				startTime = bwdEdges[i]->getStartTime();
			}
		}
	}
	return startTime;
}

int TemporalNode::getNumEdges(bool direction){
	if (direction) return numFwdEdges;
	return numBwdEdges;
}

TemporalEdge* TemporalNode::getEdgeAt(int i, bool direction){
	if (direction) return fwdEdges[i];
	return bwdEdges[i];
}

TemporalEdge* TemporalNode::sampleEdge(bool direction){
	int n = getNumEdges(direction);
	if (n > 0)
		return getEdgeAt(rand() % n, direction);
	return NULL;
}

bool TemporalNode::checkInterval(Interval *x, bool direction){
    int n = getNumEdges(direction);
    for (int i = 0; i < n; i++)
        if (getEdgeAt(i,direction)->getInterval()->doesOverlap(x)) return true;

    return false;
}
