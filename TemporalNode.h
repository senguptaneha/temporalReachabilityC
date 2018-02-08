#ifndef TEMPORALNODE_H
#define TEMPORALNODE_H

#include <vector>
#include "TemporalEdge.h"
#include "IntervalTree.h"

class TemporalNode{

public:
	TemporalNode(int isNodeId);
	IntervalTree *t;

	void addEdge(int destId, int startTime, bool direction);
	int removeEdge(int destId, int endTime, bool direction);
	int getNumEdges(bool direction);

	TemporalEdge *getEdgeAt(int i, bool direction);
	std::vector<IntervalNode*> getOverlappingEdges(Interval *i, bool direction);
	TemporalEdge *sampleEdge(bool direction);
	bool checkInterval(Interval *x, bool direction);

private:
	int nodeId;
	std::vector<TemporalEdge*> fwdEdges;
	std::vector<TemporalEdge*> bwdEdges;
	int numFwdEdges;
	int numBwdEdges;
};
#endif
